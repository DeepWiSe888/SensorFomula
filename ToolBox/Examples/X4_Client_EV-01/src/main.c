//#include <iostream>
#include <pthread.h>
//#include <condition_variable>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "taskRadar.h"
#include "vs.h"

#define DEBUG

int main(int argc, char *argv[])
{
    //vs_init(DEFAULT_FPS);
//    printf("raspbian_x4driver start to work!\n""""""\n\n");
//    for(int i = 0;i<10;i++)
//    {
//        sleep(1);
//        printf("ready for work %d s.\n",i+1);
//    }
    pthread_t ntid = 0;
    pthread_create(&ntid, 0, taskRadar, 0);
    //pthread_join(&ntid, 0);
    //std::thread taskRadarThread(taskRadar);
    //taskRadarThread.join();

    printf("raspbian_x4driver done.\n");
    while(1)
    {
        sleep(1);
        continue;
    }
    return 0;
}
