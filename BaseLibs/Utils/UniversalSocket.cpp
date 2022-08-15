//
// Created by link on 2022/6/29.
//

#include "UniversalSocket.h"

#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ifaddrs.h>

#include <errno.h>
#include <stdio.h>

#define _LINUX  (1)

#define INVLID_USOCKET_ID (0xffffffff)

USocket::USocket()
{
    socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_id<0)
        socket_id = INVLID_USOCKET_ID;
    socket_mode = USOCKET_TCP;
    socket_status = 0;
}

USocket::USocket(USOCKET_RUNMODE mode)
{
    if(mode == USOCKET_EMPTY)
        socket_id = INVLID_USOCKET_ID;
    else if(mode==USOCKET_UDP)
        socket_id = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    else if(mode==USOCKET_ICMP)
        socket_id = socket(AF_INET, SOCK_RAW, 0);
    else
        socket_id = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_id<0)
        socket_id = INVLID_USOCKET_ID;

    socket_mode = mode;
    socket_status = 0;
}

USocket::USocket(const USocket& copy)
{
    socket_id = copy.socket_id;
    socket_mode = copy.socket_mode;
    socket_status = copy.socket_status;
}

USocket::~USocket()
{
    //USocketPool::remove(this);
}

USocket& USocket::operator=(const USocket& copy)
{
    socket_id = copy.socket_id;
    socket_mode = copy.socket_mode;
    socket_status = copy.socket_status;

    return *this;
}

int USocket::reInit(USOCKET_RUNMODE mode)
{
    closeSocket();

    if(mode==USOCKET_UDP)
        socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    else if(mode==USOCKET_ICMP)
        socket_id = socket(AF_INET, SOCK_RAW, 0);
    else
        socket_id = socket(AF_INET, SOCK_STREAM, 0);

    if(socket_id<0)
        socket_id = INVLID_USOCKET_ID;

    socket_mode = mode;
    socket_status = 0;

    return 0;
}

USocket& USocket::operator=(int sockfd)
{
    socket_id = sockfd;

    socket_mode = USOCKET_TCP;
    return *this;
}

bool USocket::isValid()
{
    return (socket_id!=INVLID_USOCKET_ID);
}
bool USocket::isConnected()
{
    return (isValid())&&(socket_status&USOCKET_CONNECTED);
}

void USocket::join()
{
    //
    //USocketPool::join(this);
}

#ifdef _LINUX
int USocket::closeSocket()
{
    close(socket_id);
    socket_mode = USOCKET_EMPTY;
    socket_status = 0;
    socket_id = INVLID_USOCKET_ID;
    return 0;
}

// --- as server --- //
int USocket::bindAndListen(uint32_t port)
{
    struct sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    s_addr.sin_addr.s_addr = htonl(INADDR_ANY);


    int ret = bind(socket_id,(struct sockaddr *)&s_addr,
                  sizeof(s_addr));
    if(ret == -1)
    {
        closeSocket();
        return -1;
    }

    socket_status |= USOCKET_BINDED;



    if(socket_mode==USOCKET_TCP)
    {
        const int MAX_LISTEN_QUEUE = 8;
        ret = listen(socket_id, MAX_LISTEN_QUEUE);
        if (ret == -1)
        {
            closeSocket();
            return -2;
        }

        socket_status |= USOCKET_LISTENED;
    }

    return 0;


}
USocket USocket::acceptSocket(uint32_t timeout_ms)
{
    struct timeval tv;
    tv.tv_sec = timeout_ms/1000;
    tv.tv_usec = timeout_ms%1000*1000;

    int maxfd = socket_id+1;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(socket_id, &fds);

    int res = select(maxfd, &fds, 0, 0, &tv);
    USocket usock(USOCKET_EMPTY);

    if(res<0)
        return usock;

    if(FD_ISSET(socket_id, &fds))
    {
        struct sockaddr_in s_addr;
        socklen_t s_len = sizeof (s_addr);
        int s = accept(socket_id, (struct sockaddr*)&s_addr, &s_len);
        usock = s;
        usock.socket_status |= USOCKET_CONNECTED;
    }

    return usock;
}
// --- as client --- //
int USocket::connectServer(char* dest_ip, uint32_t port)
{
    char * destaddr = dest_ip;

    struct sockaddr_in    servaddr;
    bzero(&servaddr,sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr= inet_addr(destaddr);
    servaddr.sin_port = htons(port);

    if( connect(socket_id, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        closeSocket();
        printf("connect radar failed. err=%d, %s\n", errno, strerror(errno));
        return -2;
    }

    socket_status |= USOCKET_CONNECTED;

    return 0;
}


// --- tcp r/w --- //
int USocket::tcpSend(char* buf, int buflen)
{
    if(!isValid())
        return -1;

    //TODO:
    //select ...

    int sendsize = send(socket_id, buf, buflen, 0);

    static int debug_sendinx = 0;
    if(sendsize<0)
    {
        printf("send error %d, %s", errno, strerror(errno));
    }
    //else
    //    printf("[%d]send data size:%d\n", debug_sendinx++, buflen);

    return 0;
}
int USocket::tcpRecv(char* buf, int maxbuf, int timeout_ms)
{
    fd_set fds;
    FD_ZERO(&fds);
    int maxfd = socket_id+1;
    FD_SET(socket_id, &fds);

    struct timeval tv;
    tv.tv_sec = timeout_ms/1000;
    tv.tv_usec = timeout_ms%1000*1000;
    int res = select(maxfd, &fds, 0, 0, &tv);

    if(res<0)
        return -1;

    if(!FD_ISSET(socket_id, &fds))
        return 0;

    res = recv(socket_id, buf, maxbuf, 0);

    if(res==0)
    {
        closeSocket();
        return -2;
    }

    return res;
}


// --- udp r/w --- //
int USocket::udpSend(char* ip, uint32_t port, char* buf, int buflen)
{
    if (port < 1024 || port > 65535 || !buf || buflen <= 0)
        return -1;

    if(socket_id==INVLID_USOCKET_ID || socket_mode!=USOCKET_UDP)
        reInit(USOCKET_UDP);

    struct sockaddr_in si_other;
    socklen_t slen = 0;

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(port);
    si_other.sin_addr.s_addr = inet_addr(ip);
    slen = sizeof(si_other);

    int send_size = sendto(socket_id, buf, buflen, 0, (struct sockaddr*)&si_other, slen);
    if(send_size<0)
        printf("udp send err %d:  %s\n",errno, strerror(errno));
    return send_size;
}
int USocket::udpRecv(uint32_t port, char* buf, int maxbuf)
{
    if(socket_id==INVLID_USOCKET_ID || socket_mode!=USOCKET_UDP)
        reInit(USOCKET_UDP);
    if(!(socket_status&USOCKET_BINDED))
    {
        bindAndListen(port);
    }

    int recv_size = 0;

    struct sockaddr_in src_addr ={};
    socklen_t addr_len = sizeof(struct sockaddr_in);

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(socket_id,&readfds);
    struct timeval tv = {2, 0};
    select(socket_id+1,&readfds,NULL,NULL,&tv);
    if(FD_ISSET(socket_id,&readfds))
    {
       recv_size = recvfrom(socket_id, buf, maxbuf, 0, (struct sockaddr *) &src_addr, &addr_len);
       if(recv_size>0)
       {
           buf[recv_size]=0;
           printf("recv server info : %s\n", buf);
       }
    }

    return recv_size;
}


static char self_ip[64]={0};
int USocket::getSelfIP(char* out)
{
    if(self_ip[0])
    {
        strncpy(out, self_ip, 63);
        return 0;
    }

    // find self ip
    char tmp_ip[64]={0};
    struct ifaddrs *ifaddr, *ifa;
    int family, s;
    char host[256];
    getifaddrs(&ifaddr);
    for(ifa = ifaddr; ifa!=0; ifa=ifa->ifa_next)
    {
        if(ifa->ifa_addr==0)
            continue;
        if(memcmp(ifa->ifa_name, "lo", 2)==0)
            continue;

        if(ifa->ifa_addr->sa_family==AF_INET)
        {
            strncpy(tmp_ip, inet_ntoa( ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr ), 60);
            if(tmp_ip[0])
                strncpy(self_ip, tmp_ip, 60);
            if(memcmp(tmp_ip, "192.", 4)==0)
                break;
            //printf("[%s] %s\n", ifa->ifa_name, inet_ntoa( ((struct sockaddr_in*)ifa->ifa_addr)->sin_addr ));
        }
    }




    if(self_ip[0])
    {
        return getSelfIP(out);
    }
    else
        return -1;

}


#endif //_LINUX