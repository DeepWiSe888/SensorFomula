#include "x4.h"
#include <stdint.h>


RadarInterfaceX4::RadarInterfaceX4(BaseSensorInfo& info) : BaseSensor(info)
{

}
RadarInterfaceX4::~RadarInterfaceX4()
{

}

#include <stdio.h>
#include <unistd.h>
#include "../OldFileAccess/old_file.h"
int RadarInterfaceX4::InstanceFile()
{
    const char* filename = sensor_info.saveFilePath;
    if(filename==0 || sensor_info.onDataFun==0)
        return -1;

    ArtsEngineFileAccess aefa;
    if(aefa.open(filename)<0)
        return -1;

    x4_frame_arts frame;
    UMatC   m(DataLabel(RADAR, 138));
    int bin_cnt = 0;
    int usleep_us = sensor_info.fps?1000*1000/sensor_info.fps : 1000*1000/40;
    while (1)
    {
        int read_size = aefa.readFrame(&frame);
        if(bin_cnt==0)
            bin_cnt = aefa.frame_size/2;
        if(read_size<1)
            break;
        for(int x=0;x<138;x++)
        {
            if(x<bin_cnt)
            {
                m.At(x)->i = frame.i[x];
                m.At(x)->q = frame.q[x];
            }
            else
            {
                m.At(x)->i = 0;
                m.At(x)->q = 0;
            }
        }
        m.cap_time.unix_time_s = frame.captime&0xffffffff;
        sensor_info.onDataFun(m);
        usleep(usleep_us);
    }

    aefa.close();

    return 0;
}