#include "UniversalTcp.h"
#include <pthread.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include "UniversalThread.h"

#define TCP_DEFAULT_TIMEOUT_MS (2000)
#define TCP_MAX_CLIENT  (32)

TCPServer::TCPServer() : usocket(USOCKET_TCP), usocket_udp(USOCKET_UDP)
{
    thread_id_accept = 0;
    thread_id_server = 0;
    thread_id_broadcast = 0;
    server_ip[0] = 0;

    //client_list = new TCPLinkInfo[TCP_MAX_CLIENT];
}

TCPServer::~TCPServer()
{
    if(thread_id_accept)  delete (pthread_t*)thread_id_accept;
    if(thread_id_server)  delete (pthread_t*)thread_id_server;
    if(thread_id_broadcast)  delete (pthread_t*)thread_id_broadcast;
    //delete[] client_list;
}

int TCPServer::Instance(OnTcpData onTcpDataFun, int Port, const char* serverip)
{
    pthread_t *pid = new pthread_t;
    pthread_create(pid, 0, TCPServerThread, this);
    thread_id_server = (long)pid;
    pid = new pthread_t;
    pthread_create(pid, 0, TCPAcceptThread, this);
    thread_id_accept = (long)pid;
    pid = new pthread_t;
    pthread_create(pid, 0, BroadcastThread, this);
    thread_id_broadcast = (long)pid;

    onDataFun = onTcpDataFun;
    listen_port = Port;

    if(serverip)
        strcpy(server_ip, serverip);
    else
    {
        USocket::getSelfIP(server_ip);
    }
    if(server_ip[0]==0)
        return -1;

    return 0;
}

int TCPServer::broadcast(int flag)
{
    char local_ip[64] = {0};
    sprintf(local_ip, "%s,%d", server_ip, listen_port);
    //strcpy(local_ip, server_ip);
    printf("broad cast server addr : %s at udp port %d\n", local_ip, DEFAULT_BROADCAST_PORT);
    int optval = 1;
    setsockopt(usocket_udp.socket_id, SOL_SOCKET, SO_BROADCAST, &optval,sizeof(int));
    usocket_udp.udpSend((char*)"255.255.255.255", DEFAULT_BROADCAST_PORT, local_ip, strlen(local_ip)+1);
    return 0;
}

void* TCPServer::TCPServerThread(void* param)
{
    TCPServer* pthis = (TCPServer*)param;
    pthis->TCPServerThreadFun();
    return 0;
}

void* TCPServer::TCPAcceptThread(void* param)
{
    TCPServer* pthis = (TCPServer*)param;
    pthis->TCPAcceptThreadFun();
    return 0;

}
void* TCPServer::BroadcastThread(void* param)
{
    TCPServer* pthis = (TCPServer*)param;
    pthis->BroadcastThreadFun();
    return 0;
}

int TCPServer::TCPServerThreadFun()
{
    while (!UniversalThread::StopFlag())
    {
        if(recvClientData()<=0)
        {
            usleep(50*1000);
        }
    }

    return 0;
}


int TCPServer::TCPAcceptThreadFun()
{
    while (!UniversalThread::StopFlag())
    {
        if( (usocket.socket_status&USOCKET_LISTENED)==0)
        {
            if(usocket.bindAndListen(listen_port)<0)
            {
                printf("tcp server bind failed.\n");
                sleep(2);
                continue;
            }
            printf("tcp server bind and listened at port %d...\n", listen_port);
        }

        auto s_client = usocket.acceptSocket(2000);
        if(s_client.isValid())
        {
            printf("new client found.\n");
            addClient(s_client);
        }

    }
    return 0;
}

int TCPServer::BroadcastThreadFun()
{
    long broadcast_tick = time(0);

    while (!UniversalThread::StopFlag())
    {
        if(time(0)>(broadcast_tick+5))
        {
            broadcast(0);
            broadcast_tick = time(0);
        }
        sleep(1);
    }
    return 0;
}


int TCPServer::addClient(USocket new_client)
{
    lock.lock();
    for (auto &linkinfo : client_list )
    {
        if(linkinfo.usock.isValid())
            continue;
        linkinfo.usock = new_client;
        lock.unLock();
        return 0;
    }
    lock.unLock();
    return -1;
}

int TCPServer::recvClientData()
{
    fd_set fds;
    FD_ZERO(&fds);
    int maxfd = 0;
    lock.lock();
    for (auto &linkinfo : client_list )
    {
        if (!linkinfo.usock.isValid())
            continue;
        FD_SET(linkinfo.usock.socket_id, &fds);
        int select_fd = linkinfo.usock.socket_id +1;
        maxfd = select_fd>maxfd?select_fd:maxfd;
    }
    if(maxfd==0)
    {
        lock.unLock();
        return 0;
    }

    struct timeval tv;
    tv.tv_sec = TCP_DEFAULT_TIMEOUT_MS/1000;
    tv.tv_usec = TCP_DEFAULT_TIMEOUT_MS%1000*1000;
    int res = select(maxfd, &fds, 0, 0, &tv);
    if(res<0)
    {
        lock.unLock();
        return 0;
    }

    const int max_tmpbuf = 1536;
    char tmpbuf[1536];
    int recv_cnt = 0;

    for (auto &linkinfo : client_list )
    {
        if (FD_ISSET(linkinfo.usock.socket_id, &fds))
        {
            recv_cnt ++;
            int res = recv(linkinfo.usock.socket_id, tmpbuf, max_tmpbuf, 0);
            if(res<=0)
                linkinfo.close();
            else if (res>0)
            {
                linkinfo.addData(tmpbuf, res);
                UMatC u;
                int nRead = u.Load(linkinfo.buf, linkinfo.buf_size);
                if(nRead>0)
                {
                    linkinfo.removeData(nRead);
                    if(onDataFun) onDataFun(u, linkinfo.usock.socket_id);
                }
            }
        }
    }

    lock.unLock();
    return recv_cnt;
}


TCPClient::TCPClient() : usock(USOCKET_TCP)
{
    server_ip[0] = 0;
    server_port = DEFAULT_SERVER_PORT;
}
TCPClient::~TCPClient()
{
    usock.closeSocket();
}

int TCPClient::Instance()
{
    return 0;
}

int TCPClient::SendData(UMat& data)
{
    if(server_ip[0]==0)
        return -1;

    if(!usock.isConnected())
        usock.connectServer(server_ip, server_port);
    if(!usock.isConnected())
        return -2;

    char* tmpbuf = 0;
    int tmpsize = 0;
    data.Dump(&tmpbuf, &tmpsize);
    if(tmpsize==0 || tmpbuf==0)
        return -2;
    usock.tcpSend(tmpbuf, tmpsize);
    delete[] tmpbuf;

    return tmpsize;
}

int TCPClient::SendUmatBuf(char* bufOfUmat, int size)
{
    if(server_ip[0]==0)
        return -1;

    if(!usock.isConnected())
        usock.connectServer(server_ip, server_port);
    if(!usock.isConnected())
        return -2;

    usock.tcpSend(bufOfUmat, size);

    return size;
}

int TCPClient::findServer()
{
    char tmp_buf[128];
    char buf_size = 0;
    USocket tmp_udp(USOCKET_UDP);
    buf_size = tmp_udp.udpRecv(DEFAULT_BROADCAST_PORT, tmp_buf, 128);
    tmp_udp.closeSocket();
    if(buf_size<=0)
        return -1;

    char* comma = strstr(tmp_buf, ",");
    if(comma == nullptr)
        return -2;
    *comma = 0;
    char* szIP = tmp_buf;
    char* szPort = comma+1;
    int srvPort = atol(szPort);
    if(srvPort!=server_port)
        server_port = srvPort;

    strcpy(server_ip, szIP);

    usock.connectServer(server_ip, srvPort);

    return 0;
}


int TCPClient::connectServer(const char* ip, const int port)
{
    strcpy(server_ip, ip);
    server_port = port;

    usock.connectServer(server_ip, server_port);

    return 0;
}


int TCPClient::isConnected()
{
    return usock.isConnected()?1:0;
}