#include "UniversalTcp.h"
#include <pthread.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "UniversalThread.h"

#define TCP_MAX_CLIENT  (32)

TCPServer::TCPServer() : usocket(USOCKET_TCP), usocket_udp(USOCKET_UDP)
{
    thread_id = 0;
    server_ip[0] = 0;

    //client_list = new TCPLinkInfo[TCP_MAX_CLIENT];
}

TCPServer::~TCPServer()
{
    if(thread_id)
    {
        delete (pthread_t*)thread_id;
    }
    //delete[] client_list;
}

int TCPServer::Instance(OnTcpData onTcpDataFun, int Port, const char* serverip)
{
    pthread_t *pid = new pthread_t;
    pthread_create(pid, 0, TCPServerThread, this);

    thread_id = (long)pid;

    onDataFun = onTcpDataFun;
    listen_port = Port;

    if(serverip)
        strcpy(server_ip, serverip);
    else
    {
        USocket::getSelfIP(server_ip);
    }
    if(serverip[0]==0)
        return -1;

    return 0;
}

int TCPServer::broadcast(int flag)
{
    char local_ip[64] = {0};
    sprintf(local_ip, "%s,%d", server_ip, listen_port);
    //strcpy(local_ip, server_ip);
    usocket_udp.udpSend("255.255.255.255", DEFAULT_BROADCAST_PORT, local_ip, strlen(local_ip)+1);
    return 0;
}

void* TCPServer::TCPServerThread(void* param)
{
    TCPServer* pthis = (TCPServer*)param;

    pthis->TCPServerThreadFun();

    return 0;
}

int TCPServer::TCPServerThreadFun()
{
    usocket.bindAndListen(listen_port);
    long broadcast_tick = time(0);
    while (!UniversalThread::StopFlag())
    {
        auto s_client = usocket.acceptSocket(5000);
        if(s_client.isValid())
        {
            addClient(s_client);
        }

        if(time(0)>(broadcast_tick+10000))
        {
            broadcast(0);
        }

        recvClientData();
    }
}


int TCPServer::addClient(USocket new_client)
{
    for (auto &linkinfo : client_list )
    {
        if(linkinfo.usock.isValid())
            continue;
        linkinfo.usock = new_client;
        return 0;
    }
    return -1;
}
#include <unistd.h>
int TCPServer::recvClientData()
{
    fd_set fds;
    FD_ZERO(&fds);
    int maxfd = 0;
    for (auto &linkinfo : client_list )
    {
        if (linkinfo.usock.isValid())
            continue;
        FD_SET(linkinfo.usock.socket_id, &fds);
        int select_fd = linkinfo.usock.socket_id +1;
        maxfd = select_fd>maxfd?select_fd:maxfd;
    }
    if(maxfd==0)
        return 0;

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    int res = select(maxfd, &fds, 0, 0, &tv);

    const int max_tmpbuf = 1536;
    char tmpbuf[1536];

    for (auto &linkinfo : client_list )
    {
        if (FD_ISSET(linkinfo.usock.socket_id, &fds))
        {
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
                    if(onDataFun) onDataFun(u);
                }
            }
        }
    }


    return 0;
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

int TCPClient::Instance(int Port)
{
    server_port = Port;
    return 0;
}

int TCPClient::SendData(UMat& data)
{
    if(server_ip[0]==0)
        return -1;

    char* tmpbuf = 0;
    int tmpsize = 0;
    data.Dump(&tmpbuf, &tmpsize);
    if(tmpsize==0 || tmpbuf==0)
        return -2;
    usock.tcpSend(tmpbuf, tmpsize);
    delete[] tmpbuf;

    return tmpsize;
}
#include <stdlib.h>
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
    if(comma==0)
        return -2;
    *comma = 0;
    char* szIP = tmp_buf;
    char* szPort = comma+1;
    int srvPort = atol(szPort);
    if(srvPort!=server_port)
        server_port = srvPort;

    strcpy(server_ip, szIP);

    return 0;
}