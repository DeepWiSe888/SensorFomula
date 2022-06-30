#ifndef _SENSOR_FORMULA_BASELIBS_TCP_H
#define _SENSOR_FORMULA_BASELIBS_TCP_H

#include "../UniversalData/UniversalData.h"

const static short DEFAULT_SERVER_PORT = 15646;

typedef int(*OnTcpData)(UMat data);

class TCPServer
{
public:
    TCPServer();
    ~TCPServer();

public:
    int Instance(int Port = DEFAULT_SERVER_PORT, OnTcpData onDataFun);

    int broadcast(int flag);
};

class TCPClient
{
public:
    TCPClient();
    ~TCPClient();

public:
    int Instance(int Port = DEFAULT_SERVER_PORT);

    void SendData(UMat& data);

public:
    int findServer();
};


#endif