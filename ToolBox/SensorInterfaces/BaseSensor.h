#ifndef _SENSOR_FORMULA_INTERFACES_BASE_H
#define _SENSOR_FORMULA_INTERFACES_BASE_H

#include "../../BaseLibs/UniversalData/UniversalData.h"

typedef int(*OnDataCallback)(UMat data);


typedef struct _BaseSensorInfo
{
    OnDataCallback  onDataFun;      // callback function for data output
    char*           saveFilePath;   // FileMode: Read this file; OtherMode: save file if path is not NUll
    int             periodLevel;    // 0:Highest ~ 5:Lowest
    //Reserved for ROS
    void*           reservedForROS;
} _BaseSensorInfo;

class BaseSensor
{
public:
    BaseSensor(OnDataCallback onDataFun);
    ~BaseSensor();

public:
    virtual int InstanceIO(void* param)=0;
    virtual int InstanceFile()=0;
    virtual int InstanceTcpSrv()=0;
    virtual int InstanceSimulate()=0;

    virtual void Pause(){};
    virtual void Release()=0;
};

#endif