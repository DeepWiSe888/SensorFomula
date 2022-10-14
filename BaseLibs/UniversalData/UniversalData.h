#ifndef _SENSOR_FORMULA_UNIVERSAL_DATA_H_
#define _SENSOR_FORMULA_UNIVERSAL_DATA_H_

#include <stdint.h>
#include "BaseDataDefine.h"

#pragma pack(1)

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

#ifndef DATA_TYPE_
#define DATA_TYPE_
// hint: you should use RADAR|CAMERA while describing data streams you want
#define DATA_TYPE_UNDEFINED     (0)
#define DATA_TYPE_ADC           (1)
#define DATA_TYPE_IQ            (2)
#define DATA_TYPE_IMAGE         (3)
#define DATA_TYPE_POINTCLOUD    (4)
#endif

#define DATALABEL_VER   (1)

typedef struct _DataLabel
{
    uint8_t     version;    // version = DATALABEL_VER
    uint8_t     sensorType;
    uint8_t     dataType;
    //uint8_t frameDim; // MIMO Radar: TX*RX*fastTime = 3 dims
                        // SISO Radar: fastTime = 1 dims
                        // Cam/Lidar: H*W = 2 dims
    uint8_t floatSize;  // 4 for float, or 8 for double
    uint32_t dims[4];
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
        dataType = DATA_TYPE_UNDEFINED;
        floatSize = sizeof(UF);
        *((uint32_t*)(&dims[0])) = 0;
    }

    // Example dataQueue.import(data, DataLabel(MIMO, 3, 4, 256));
    _DataLabel(uint8_t _sensorType, uint8_t dim0, uint8_t dim1=0, uint8_t dim2=0, uint8_t dim3=0 )
    {
        version = DATALABEL_VER;
        sensorType = _sensorType;
        dataType = DATA_TYPE_UNDEFINED;
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

#pragma pack()

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

typedef int(*onDimData)(Complex* out, int i1, int i2, int i3, int i4, void* context);


class UMatC
{
protected:
    DataLabel label;
    matc*    mat;
public:
    SensorTimeStamp cap_time;
    int             fps;
public:
    UMatC();
    UMatC(DataLabel label);
    UMatC(const UMatC& copy);
    ~UMatC();

    UMatC& operator=(const UMatC& copy);

public:
    /// ------- combination and increase -------- ///

    /// dim N sequence dim N : create a new mat dim N+1 as [2, ...]     ///
    UMatC& sequence(UMatC& in);
    /// dim N+1 append dim N : increase dim0 as [dim0+1, ...]           ///
    UMatC& append(UMatC& in);

public:
     /// --------  Access Operations -------- ///

     /// return data ptr, value can be change ///
    Complex* At(int i1, int i2=0, int i3=0, int i4=0);

    /// read one dim data for each other dimentions.                      ///
    /// almoust use for matrix caculation, such as fft, filters.          ///
    /// at callback function, caculation dim index(i1...i4) will be -1.   ///
    /// context can use for lambda context 'capture'.                     ///
    /// see examples at BLAS/fourier.cpp                                  ///
    int getDimData(int dim_inx, onDimData on_data, void* context);

    /// updated corresponding dim index(i1...i4) = -1. ///
    /// see examples at BLAS/fourier.cpp               ///
    int updateDimData(Complex* in, int dim_inx, int i1, int i2, int i3, int i4);

    
public:
    /// UMat is not designed for strict safety, so internal data can be access ///
    /// sometimes it's faster by memory operations.                            ///
    matc * getMat(){return mat;}
    DataLabel* getLable(){return &label;}


public:
    /// --------   Memory / File Buffer Operations  -------- ///
    // return: *outSize
    int Dump(char** outBuf, int *outSize);

    // return: read buf size if succeed; 0 if failed; -n if error.
    int Load(char* inBuf, int bufSize);

public:
    /// --- other operations --- ///

    // get current time as data timestamp
    void updateTimeStamp();

protected:
    void clear();
};

#define     UMat    UMatC

#endif