
#include <stdio.h>
#include "UniversalData.h"
#include "UniversalTcp.h"
#include "Radar/x4.h"
#include "UniversalTcp.h"
#include <unistd.h>

#include "fourier.h"

//#define _TCP_OUT

TCPClient   tcp_client;

UMatC *frame_data = 0;
int frame_tick = 0;

int onData(UMatC m)
{
    frame_tick++;
    if(frame_tick==1)
    {
        frame_data = new UMatC(m);
    }
    else if(frame_tick==2)
    {
        frame_data->sequence(m);
    }
    else
        frame_data->append(m);

    printf("shape:%d, %d, %d\n", frame_data->getMat()->dims[0], frame_data->getMat()->dims[1], frame_data->getMat()->dims[2]);

#ifdef _TCP_OUT
    tcp_client.SendData(m);
#endif
    return 0;
}

void runFile()
{
    char* filename = const_cast<char*>("/Users/link/Projects/data/vpas/a_20211229-020000.dat");
    int fps = 10000;
    BaseSensorInfo bsi={onData, filename, 0, fps, 0};
    RadarInterfaceX4  x4file(bsi);

    x4file.InstanceFile();
}

void processDataTest()
{
    UMatC rdmat;
    //UMatC iqmat = *frame_data;
    sfblas::fft(*frame_data, rdmat, 1024, 0);
    for(int i=0;i<100;i++)
    {
        printf("%f+%fi, ", rdmat.At(i, 0)->i, rdmat.At(i, 0)->q);
        if(i&&!(i%10))
            printf("\n");
    }
    printf("\n");
}


int main(int argc, char **argv)
{
    char self_ip[32] = {0};
    USocket::getSelfIP(self_ip);
    printf("local ip = %s\n", self_ip);

#ifdef _TCP_OUT
    printf("find server...\n");
    tcp_client.Instance();
    int find_server_tick = 0;
    while(tcp_client.findServer()<0)
    {
        if(find_server_tick++>15)
        {
            //tcp_client.close();
            printf("radar server not found.\n");
            return -1;
        }
        usleep(1000);
    }
    printf("server found.\n");
#endif
    printf("x4 start ... \n");
    runFile();
    processDataTest();
    UniversalThread::StopAllThreads();
    return 0;
}