#ifndef _SENSOR_FORMULA_BASELIBS_VERSION_H
#define _SENSOR_FORMULA_BASELIBS_VERSION_H

#include <stdio.h>
static unsigned long SF_MAJOR_VER = 1;
static unsigned long SF_MINOR_VER = 1;
static unsigned long SF_BUILD_VER = 3;
char ver_buf[64] = {0};

char* sf_version()
{
    if(ver_buf[0]==0)
    sprintf(ver_buf, "%lu.%lu.%lu", SF_MAJOR_VER, SF_MINOR_VER, SF_BUILD_VER);
    return ver_buf;
}

#endif