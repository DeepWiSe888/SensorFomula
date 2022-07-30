
#include <stdio.h>
#include "UniversalData.h"
#include "UniversalTcp.h"
#include "Radar/x4.h"


int onData(UMatC m)
{
    printf("recv frame :%d\n", 1);
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
    printf("x4 start ... \n");
    runFile();
    return 0;
}