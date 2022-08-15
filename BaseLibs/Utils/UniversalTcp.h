#ifndef _SENSOR_FORMULA_BASELIBS_TCP_H
#define _SENSOR_FORMULA_BASELIBS_TCP_H

#include "../UniversalData/UniversalData.h"
#include "UniversalSocket.h"
#include "UniversalThread.h"
#include <string.h>


const static short DEFAULT_SERVER_PORT = 15646;
const static short DEFAULT_BROADCAST_PORT = 15647;

const static char* DEFAULT_SERVER_IP = 0;

typedef int(*OnTcpData)(UMat&);



typedef struct _TCPLinkInfo
{
    USocket usock;
    char*   buf;
    int     buf_size;
    int     max_buf;

    _TCPLinkInfo()
    {
        usock = USocket(USOCKET_EMPTY);
        max_buf = 1024*64;
        buf = new char[max_buf];
        buf_size = 0;
    }
    ~_TCPLinkInfo()
    {
        delete[] buf;
        buf = 0;
        max_buf = 0;
        buf_size = 0;
    }

    void close()
    {
        usock.closeSocket();
        usock = USocket(USOCKET_EMPTY);
        buf_size = 0;
    }

    void addData(char* data ,int size)
    {
        if(size + buf_size > max_buf)
            buf_size = 0;
        memcpy(buf+ buf_size, data, size);
        buf_size += size;
    }

    void removeData(int size)
    {
        if(size>=buf_size)
            buf_size = 0;
        else
        {
            memmove(buf, buf+size, buf_size-size);
            buf_size -= size;
        }
    }

} TCPLinkInfo;



class TCPServer
{
private:
    USocket     usocket;
    USocket     usocket_udp;
    OnTcpData   onDataFun;
    short       listen_port;
    char        server_ip[32];

protected:
    long        thread_id_server;
    long        thread_id_accept;
    long        thread_id_broadcast;

protected:
    TCPLinkInfo client_list[32];
    ULock       lock;

public:
    TCPServer();
    ~TCPServer();

public:
    int Instance(OnTcpData onTcpDataFun, int Port = DEFAULT_SERVER_PORT, const char* serverIP=DEFAULT_SERVER_IP);

    int broadcast(int flag);

public:
    static void* TCPServerThread(void* param);
    static void* TCPAcceptThread(void* param);
    static void* BroadcastThread(void* param);

protected:
    int TCPServerThreadFun();
    int TCPAcceptThreadFun();
    int BroadcastThreadFun();
    int addClient(USocket new_client);
    int recvClientData();
};

class TCPClient
{
private:
    USocket usock;
    char    server_ip[64];
    int     server_port;
public:
    TCPClient();
    ~TCPClient();

public:
    int Instance(int Port = DEFAULT_SERVER_PORT);

    int SendData(UMat& data);

public:
    int findServer();
};


#endif