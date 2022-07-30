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
} BaseSensorInfo;

class BaseSensor
{
protected:
    BaseSensorInfo sensor_info;
public:
    BaseSensor(BaseSensorInfo& info){sensor_info = info;};
    ~BaseSensor(){};

public:
    virtual int InstanceIO(void* param){return 0;}
    virtual int InstanceFile(){return 0;}
    virtual int InstanceTcpSrv(){return 0;}
    virtual int InstanceSimulate(){return 0;}
    virtual int InstanceDataStream(){return 0;}

    virtual void Pause(){};
    virtual void Release(){};
};

#endif