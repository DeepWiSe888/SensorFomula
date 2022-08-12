#ifndef _SENSOR_FORMULA_INTERFACES_OLDFILE_ACCESS_H
#define _SENSOR_FORMULA_INTERFACES_OLDFILE_ACCESS_H

#include <stdint.h>

class OldFileBase
{
protected:
    long fp;
public:
    int  frame_size;    // bincnt*2 for radar iq; row*col for cam, etc...
public:
    OldFileBase();
    ~OldFileBase();

public:
    virtual int open(const char* filename);
    virtual void close();
    //virtual int readFrame(int *frameNo, float *out, int* outSize){return 0;};
};




/******************************************************
 *
 *  ArtsEngine File Access
 *
 ******************************************************/

#pragma pack(1)
typedef struct _x4_frame_arts
{
    uint64_t    captime;
    uint32_t    fno;
    uint16_t    reserved;
    uint16_t    bin_cnt;
    uint64_t    notuse;
    float       i[138];
    float       q[138];
}x4_frame_arts;
#pragma pack()

class ArtsEngineFileAccess : public OldFileBase
{
public:
    ArtsEngineFileAccess();
    ~ArtsEngineFileAccess();

public:
    //virtual int readFrame(int *frameno, float *out, int* outSize);
    int readFrame(x4_frame_arts* out);
};



#endif