
#include <stdio.h>
#include "UniversalTcp.h"
#include "UniversalThread.h"
#include <unistd.h>

int onTCPData(UMatC& m)
{
    return 0;
}

int runServer()
{
    printf("radar server start ... \n");
    TCPServer tcp_server;
    tcp_server.Instance(onTCPData);
    while (!UniversalThread::StopFlag())
    {
        sleep(1);
    }
    return 0;
}

int main(void)
{
    runServer();
    return 0;
}