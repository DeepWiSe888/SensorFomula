#ifndef _SENSOR_FORMULA_INTERFACES_RADAR_X4_H
#define _SENSOR_FORMULA_INTERFACES_RADAR_X4_H

#include "../BaseSensor.h"

class RadarInterfaceX4 : public BaseSensor
{
public:
    RadarInterfaceX4(BaseSensorInfo& info);
    ~RadarInterfaceX4();

public:
    int InstanceFile();
};

#endif