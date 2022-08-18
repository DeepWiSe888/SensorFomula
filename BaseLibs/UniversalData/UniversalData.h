#ifndef _SENSOR_FORMULA_UNIVERSAL_DATA_H_
#define _SENSOR_FORMULA_UNIVERSAL_DATA_H_

#include <stdint.h>
#include "BaseDataDefine.h"


typedef struct
{
    uint32_t unix_time_s;
    uint32_t time_ms;
}SensorTimeStamp;

#ifndef SENSOR_TYPE_
#define SENSOR_TYPE_
// hint: you should use RADAR|CAMERA while describing data streams you want
#define RADAR     (1)
#define LIDAR     (2)      //(1<<1)
#define THERMAL   (4)      //(1<<2)
#define CAMERA    (8)      //(1<<3)
#endif

#define DATALABEL_VER   (1)

typedef struct _DataLabel
{
    uint8_t     version;    // version = DATALABEL_VER
    uint8_t     sensorType;
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
        version = DATALABEL_VER;
        sensorType = 0;
        floatSize = sizeof(UF);
        *((uint32_t*)(&dims[0])) = 0;
    }

    // Example dataQueue.import(data, DataLabel(MIMO, 3, 4, 256));
    _DataLabel(uint8_t _sensorType, uint8_t dim0, uint8_t dim1=0, uint8_t dim2=0, uint8_t dim3=0 )
    {
        version = DATALABEL_VER;
        sensorType = _sensorType;
        floatSize = sizeof(UF);
        dims[0] = dim0;
        dims[1] = dim1;
        dims[2] = dim2;
        dims[3] = dim3;
    }

    int MatSize()
    {
        int dimcnt = dimCnt();
        int i;
        int matsize = 0;
        for(i=0;i<dimcnt;i++)
        {
            if(matsize==0) matsize = 1;
            matsize = matsize*dims[i];
        }
        return matsize;
    }

}tagDataLabel, DataLabel;

#if 0
class UMatR
{
protected:
    matr*    data;
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
#endif




/*****************************************
 *  Universal Matrix
 *  Support 1~4 Dims
 *****************************************/
class UMatC
{
protected:
    DataLabel label;
    matc*    mat;
public:
    SensorTimeStamp cap_time;
public:
    UMatC();
    UMatC(DataLabel label);
    UMatC(UMatC& copy);
    ~UMatC();

    UMatC& operator=(UMatC& copy);

public:
    // dim N sequence dim N : create a new mat dim N+1 as [2, ...]
    UMatC& sequence(UMatC& in);
    // dim N+1 append dim N : increase dim0 as [dim0+1, ...]
    UMatC& append(UMatC& in);

public:

     /// --------  Access Operations -------- ///
    Complex* At(int i1, int i2=0, int i3=0, int i4=0);
    //Complex& operator[](int i1);
    //Complex& operator[](int i1, int i2);
    //Complex& operator[](int i1, int i2, int i3);
    //Complex& operator[](int i1, int i2, int i3, int i4);

    matc * getMat(){return mat;}


public:
    /// --------   Memory / File Buffer Operations  -------- ///
    // return: *outSize
    int Dump(char** outBuf, int *outSize);

    // return: read buf size if succeed; 0 if failed; -n if error.
    int Load(char* inBuf, int bufSize);
};

#define     UMat    UMatC

#endif