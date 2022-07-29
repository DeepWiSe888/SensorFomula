#ifndef _SENSOR_FORMULA_INTERFACES_SENSORFUSION_H
#define _SENSOR_FORMULA_INTERFACES_SENSORFUSION_H

#include "../../BaseLibs/UniversalData/UniversalData.h"


#ifndef SENSOR_CHANNEL_
#define SENSOR_CHANNEL_
#define SCH_IO          (1)
#define SCH_FILE        (2)
#define SCH_TCP         (4)
#define SCH_SIMULATE    (8)
#endif

class SensorChannel
{
protected:
    int channelID;
    int sensorType;
    int sensorChannel;

public:
    SensorChannel();
    ~SensorChannel();
};

typedef int(*OnNewChannel)(SensorChannel*);
typedef int(*OnChannelData)(int /*channelID*/, UMat data);

class SensorFusion
{
protected:
    SensorFusion();
    ~SensorFusion();

public:
    static SensorFusion* GetSensorFusion();

public:
    int Instance(OnNewChannel onNewChannelFun, OnChannelData onChannelDataFun);
};


#endif