
#include <stdio.h>
#include "UniversalData.h"
#include "UniversalTcp.h"
#include "Radar/x4.h"
#include "UniversalTcp.h"
#include <unistd.h>

TCPClient   tcp_client;

int onData(UMatC m)
{
    printf("recv frame :%d\n", 1);

    tcp_client.SendData(m);
    return 0;
}

void runFile()
{
    char* filename = const_cast<char*>("/home/pi/data/a.dat");
    BaseSensorInfo bsi={onData, filename, 0, 0};
    RadarInterfaceX4  x4file(bsi);

    x4file.InstanceFile();
}


int main(void)
{
    char self_ip[32] = {0};
    USocket::getSelfIP(self_ip);
    printf("local ip = %s\n", self_ip);

    printf("find server...\n");
    tcp_client.Instance();
    int find_server_tick = 0;
    while(tcp_client.findServer()<0)
    {
        if(find_server_tick++>10)
        {
            //tcp_client.close();
            return -1;
        }
        sleep(2);
    }
    printf("server found.\n");
    printf("x4 start ... \n");
    runFile();
    return 0;
}