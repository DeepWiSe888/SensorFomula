#include "sf_client.h"
#include <stdio.h>
#include <unistd.h>

#include "UniversalData.h"
#include "UniversalTcp.h"

TCPClient   tcp_client;
UMatC       m;

int sf_client_init()
{
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
    return 0;
}

int send_frame(float * fdata, int fdata_cnt)
{
    int bin_cnt = fdata_cnt/2;
    if(m.getMat()==0)
    {
        m = UMatC(DataLabel(RADAR, bin_cnt));
    }
    m.updateTimeStamp();
    for(int i=0;i<bin_cnt;i++)
    {
        m.At(i)->i = fdata[i];
        m.At(i)->q = fdata[i+bin_cnt];
    }
    tcp_client.SendData(m);
    return 0;
}