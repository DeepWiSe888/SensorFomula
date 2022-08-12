#include "UniversalData.h"
#include <string.h>
#include <stdio.h>

UMatC::UMatC()
{
    data = 0;
    cap_time={0,0};
}


UMatC::UMatC(DataLabel l)
{
    this->label = l;
    int dimcnt = l.dimCnt();
    switch(dimcnt)
    {
        case 1:
            data = createMat1C(l.dims[0]);
            break;
        case 2:
            data = createMat2C(l.dims[0], l.dims[1]);
            break;
        case 3:
            data = createMat3C(l.dims[0], l.dims[1], l.dims[2]);
            break;
        case 4:
            data = createMat4C(l.dims[0], l.dims[1], l.dims[2], l.dims[3]);
            break;
        default:
            data = 0;
            break;
    }

    cap_time={0,0};
}

UMatC::~UMatC()
{
    data = freeMat(data);
}


Complex* UMatC::At(int i1, int i2, int i3, int i4)
{
    int dimCnt = label.dimCnt();
    if(dimCnt==1)
        return &M1V(data, i1);
    else if(dimCnt==2)
        return &M2V(data, i1, i2);
    else if(dimCnt==3)
        return &M3V(data, i1, i2, i3);
    else if(dimCnt==4)
        return &M4V(data, i1, i2, i3, i4);
    else
        return 0;
}
//Complex& UMatC::operator[](int i1, int i2=0, int i3=0, int i4=0)
//{
//    return *At(i1, i2, i3, i4);
//}


#define UMAT_DUMP_HEAD_V1   "#sf01#umat"

// return: *outSize
int UMatC::Dump(char** outBuf, int *outSize)
{
    // dump title
    const int TITLE_LEN = 64;
    int matsize = getMatSize(data);
    int bufsize = TITLE_LEN + +sizeof(label) + sizeof(Complex)*matsize;
    char* newbuf = new char[bufsize];
    char* title = newbuf;
    sprintf(title, UMAT_DUMP_HEAD_V1"#v%02d#s%02d#%d,%d,%d,%d", label.version,label.sensorType, label.dims[0], label.dims[1], label.dims[2], label.dims[3]);

    // dump label
    void* plabel = newbuf + TITLE_LEN;
    memcpy(plabel, &label, sizeof (label));
    // dump data
    void* pdata = newbuf+TITLE_LEN+sizeof(label);
    memcpy(pdata,  data->data,  sizeof(Complex)*matsize);

    *outBuf = newbuf;
    *outSize = bufsize;
    return bufsize;
}



// return: read buf size if succeed; 0 if failed; -n if error.
int UMatC::Load(char* inBuf_, int bufSize)
{
    char* inBuf = (char*)inBuf_;
    const int TITLE_LEN = 64;
    if(bufSize<TITLE_LEN+sizeof(tagDataLabel))
        return 0;

    int skip_size = 0;
    int i;
    for(i=0;i<bufSize-TITLE_LEN;i++)
    {
        if(memcmp(inBuf+i, UMAT_DUMP_HEAD_V1, 10)==0)
            break;
        skip_size++;
    }
    inBuf += skip_size;
    //inBuf =((char *)inBuf) + skip_size;
    bufSize -= skip_size;
    if(memcmp(inBuf, UMAT_DUMP_HEAD_V1, 10)!=0)
        return 0;

    char* title = inBuf;
    tagDataLabel * plabel = (tagDataLabel*)(inBuf+TITLE_LEN);
    // ToDo: check label is valid
    //if(plabel->dimCnt()>4)
    //    return -2;

    Complex * pdata = (Complex*)(inBuf+TITLE_LEN+sizeof(tagDataLabel));
    int matsize = plabel->MatSize();
    if(bufSize<TITLE_LEN+sizeof(tagDataLabel)+sizeof(Complex)*matsize)
        return 0;

    matc* m = createMatC(plabel->dims[0], plabel->dims[1], plabel->dims[2],plabel->dims[3]);
    memcpy(m->data, pdata, sizeof(tagDataLabel)+sizeof(Complex)*matsize);
    memcpy(&label, plabel, sizeof (label));

    return skip_size+TITLE_LEN+sizeof(tagDataLabel)+sizeof(Complex)*matsize;
}