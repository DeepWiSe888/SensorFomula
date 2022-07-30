#include "x4.h"
#include <stdint.h>

#pragma pack(1)
typedef struct _x4_frame_file
{
    char        flag[16];
    uint32_t    fno;
    uint32_t    ant_id;
    uint32_t    bin_cnt;
    float       i[138];
    float       q[138];
}x4_frame;
#pragma pack()


RadarInterfaceX4::RadarInterfaceX4(BaseSensorInfo& info) : BaseSensor(info)
{

}
RadarInterfaceX4::~RadarInterfaceX4()
{

}

#include <stdio.h>
int RadarInterfaceX4::InstanceFile()
{
    const char* filename = sensor_info.saveFilePath;
    if(filename==0 || sensor_info.onDataFun==0)
        return -1;

    FILE* fp = fopen(filename, "rb");
    if(fp==0)
        return -1;
    x4_frame frame;
    UMatC   m(DataLabel(RADAR, 138));
    while (1)
    {
        int read_size = fread(&frame, sizeof (frame), 1, fp);
        if(read_size<sizeof (frame))
            break;
        for(int x=0;x<138;x++)
        {
            m.At(x)->i = frame.i[x];
            m.At(x)->q = frame.q[x];
        }

        sensor_info.onDataFun(m);
    }


    fclose(fp);


    return 0;
}