#ifndef _SENSOR_FORMULA_UNIVERSAL_DATA_H_
#define

#include <stdint.h>
#include "BaseDataDefine.h"



#ifndef SENSOR_TYPE_
#define SENSOR_TYPE_
#define RADAR     (1)
#define LIDAR     (2)
#define THERMAL   (3)
#define CAMERA    (4)
#define

typedef struct _DataLabel
{
    uint8_t sensorType;
    //uint8_t frameDim; // MIMO Radar: TX*RX*fastTime = 3 dims
                        // SISO Radar: fastTime = 1 dims
                        // Cam/Lidar: H*W = 2 dims
    uint8_t floatSize;  // 4 for float, or 8 for double
    uint8_t dims[4];
    uint8_t dimCnt()
    {
        if(dims[3]) return 4;
        else if(dims[2]) return 3;
        else if(dims[1]) return 2;
        else if(dims[0]) return 1;
        else return  0;
    }

    _DataLabel()
    {
        sensorType = 0;
        floatSize = sizeof(UF);
        *((uint32_t*)(&dims[0])) = 0;
    }

    // Example dataQueue.import(data, DataLabel(MIMO, 3, 4, 256));
    _DataLabel(uint8_t _sensorType, uint8_t dim0, uint8_t dim1=0, uint8_t dim2=0, uint8_t dim3=0 )
    {
        sensorType = _sensorType;
        dims[0] = dim0;
        dims[1] = dim1;
        dims[2] = dim2;
        dims[3] = dim3;
    }
}tagDataLabel, DataLabel;

/*
class UMat
{
protected:
    DataLabel   dataLabel;
    uint16_t    fps;
public:
    UMat();
    ~UMat();

public:
    int ImportData(UF * in, tagDataLabel* label);

public:
    //virtual UF* At(int i1, int i2=0, int i3=0, int i4=0);
    //virtual UF& operator[](int i1, int i2=0, int i3=0, int i4=0);
};
 */

class UMatR //: public UMat
{
protected:
    matr    data;
public:
    UMatR();
    ~UMatR();

public:
    UF* At(int i1, int i2=0, int i3=0, int i4=0);
    UF& operator[](int i1, int i2=0, int i3=0, int i4=0);
    UMatR operator+(UMatR& mat);
    UMatR operator-(UMatR& mat);
    UMatR operator*(UMatR& mat);
};

class UMatC //: public UMat
{
protected:
    matc    data;
public:
    UMatC();
    ~UMatC();

public:
    Complex* At(int i1, int i2=0, int i3=0, int i4=0);
    Complex& operator[](int i1, int i2=0, int i3=0, int i4=0);
};

#define     UMat    UMatC

#endif