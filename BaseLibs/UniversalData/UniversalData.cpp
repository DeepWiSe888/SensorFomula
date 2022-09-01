#include "UniversalData.h"
#include <string.h>
#include <stdio.h>

UMatC::UMatC()
{
    mat = 0;
    cap_time={0,0};
}


UMatC::UMatC(DataLabel l)
{
    this->label = l;
    int dimcnt = l.dimCnt();
    switch(dimcnt)
    {
        case 1:
            mat = createMat1C(l.dims[0]);
            break;
        case 2:
            mat = createMat2C(l.dims[0], l.dims[1]);
            break;
        case 3:
            mat = createMat3C(l.dims[0], l.dims[1], l.dims[2]);
            break;
        case 4:
            mat = createMat4C(l.dims[0], l.dims[1], l.dims[2], l.dims[3]);
            break;
        default:
            mat = 0;
            break;
    }

    cap_time={0,0};
}


UMatC::UMatC(const UMatC& copy)
{
    label = copy.label;
    mat = copyMat(copy.mat);
    cap_time = copy.cap_time;
}

UMatC& UMatC::operator=(const UMatC& copy)
{
    clear();
    label = copy.label;
    mat = copyMat(copy.mat);
    cap_time = copy.cap_time;

    return *this;
}

UMatC::~UMatC()
{
    if(mat)
        mat = freeMat(mat);
}

void UMatC::clear()
{
    if(mat)
        mat = freeMat(mat);
    bzero(label.dims, sizeof(label.dims));
}


Complex* UMatC::At(int i1, int i2, int i3, int i4)
{
    int dimCnt = label.dimCnt();
    if(dimCnt==1)
        return &M1V(mat, i1);
    else if(dimCnt==2)
        return &M2V(mat, i1, i2);
    else if(dimCnt==3)
        return &M3V(mat, i1, i2, i3);
    else if(dimCnt==4)
        return &M4V(mat, i1, i2, i3, i4);
    else
        return 0;
}
//Complex& UMatC::operator[](int i1, int i2=0, int i3=0, int i4=0)
//{
//    return *At(i1, i2, i3, i4);
//}


//typedef int(*onDimData)(Complex* out, int i1, int i2, int i3, int i4);
int UMatC::getDimData(int dim_inx, onDimData on_data, void* context)
{
    int dim_cnt = label.dimCnt();
    if(dim_inx<0 || dim_inx>3 || on_data==0)
        return -1;

    uint32_t i=0,j=0,k=0,l=0;

    uint32_t len = label.dims[dim_inx];
    Complex * out = new Complex [len];

    if(dim_inx==0)
    {
        if(dim_cnt==1)
        {
            for(i=0;i<len;i++) out[i] = *At(i);
            on_data(out, -1, 0, 0, 0, context);
            //continue;
        }
        for(j=0;j<label.dims[1];j++)
        {
            if(dim_cnt==2)
            {
                for(i=0;i<len;i++) out[i] = *At(i, j);
                on_data(out, -1, j, 0, 0, context);
                continue;
            }
            for(k=0;k<label.dims[2];k++)
            {
                if(dim_cnt==3)
                {
                    for(i=0;i<len;i++) out[i] = *At(i, j, k);
                    on_data(out, -1, j, k, 0, context);
                    continue;
                }
                for(l=0;l<label.dims[3];l++)
                {
                    for(i=0;i<len;i++) out[i] = *At(i, j, k, l);
                    on_data(out, -1, j, k, l, context);
                }
            }
        }
    }


    if(dim_inx==1)
    {
        for(i=0;i<label.dims[0];i++)
        {
            if(dim_cnt==2)
            {
                for(j=0;j<len;j++) out[j] = *At(i, j);
                on_data(out, i, -1, 0, 0, context);
                continue;
            }
            for(k=0;k<label.dims[2];k++)
            {
                if(dim_cnt==3)
                {
                    for(j=0;j<len;j++) out[j] = *At(i, j, k);
                    on_data(out, i, -1, k, 0, context);
                    continue;
                }
                for(l=0;l<label.dims[3];l++)
                {
                    for(j=0;j<len;j++) out[j] = *At(i, j, k, l);
                    on_data(out, i, -1, k, l, context);
                }
            }
        }
    }


    if(dim_inx==2)
    {
        for(i=0;i<label.dims[0];i++)
        {
            for(j=0;j<label.dims[1];j++)
            {
                if(dim_cnt==3)
                {
                    for(k=0;k<len;k++) out[k] = *At(i, j, k);
                    on_data(out, i, j, -1, 0, context);
                    continue;
                }
                for(l=0;l<label.dims[3];l++)
                {
                    for(k=0;k<len;k++) out[k] = *At(i, j, k, l);
                    on_data(out, i, j, -1, l, context);
                }
            }
        }
    }

    if(dim_inx==3)
    {
        for(i=0;i<label.dims[0];i++)
        {
            for(j=0;j<label.dims[1];j++)
            {
                for(k=0;k<label.dims[2];k++)
                {
                    for(l=0;l<len;l++) out[l] = *At(i, j, k, l);
                    on_data(out, i, j, k, -1, context);
                }
            }
        }
    }

    delete[] out;

    return 0;
}



int UMatC::updateDimData(Complex* in, int dim_inx, int i1, int i2, int i3, int i4)
{
    if(dim_inx<0||in==0)
        return -1;
    int len = label.dims[dim_inx];

    if(dim_inx==0)
        for(int n=0;n<len;n++) *At(n, i2, i3, i4)=in[n];
    if(dim_inx==1)
        for(int n=0;n<len;n++) *At(i1, n, i3, i4)=in[n];
    if(dim_inx==2)
        for(int n=0;n<len;n++) *At(i1, i2, n, i4)=in[n];
    if(dim_inx==3)
        for(int n=0;n<len;n++) *At(i1, i2, i3, n)=in[n];

    return 0;
}












#define UMAT_DUMP_HEAD_V1   "#sf01#umat"

// return: *outSize
int UMatC::Dump(char** outBuf, int *outSize)
{
    // dump title
    const int TITLE_LEN = 64;
    int matsize = getMatSize(mat);
    int bufsize = TITLE_LEN + +sizeof(label) + sizeof(cap_time) + sizeof(Complex)*matsize;
    char* newbuf = new char[bufsize];
    char* title = newbuf;
    sprintf(title, UMAT_DUMP_HEAD_V1"#v%02d#s%02d#%d,%d,%d,%d", label.version,label.sensorType, label.dims[0], label.dims[1], label.dims[2], label.dims[3]);

    // dump label
    void* plabel = newbuf + TITLE_LEN;
    memcpy(plabel, &label, sizeof (label));
    // dump timestamp
    void* pTime = newbuf + TITLE_LEN + sizeof(label);
    memcpy(pTime, &cap_time, sizeof (cap_time));
    // dump data
    void* pdata = newbuf+TITLE_LEN+sizeof(label) + sizeof (cap_time);
    memcpy(pdata,  mat->data,  sizeof(Complex)*matsize);

    *outBuf = newbuf;
    *outSize = bufsize;
    return bufsize;
}



// return: read buf size if succeed; 0 if failed; -n if error.
int UMatC::Load(char* inBuf_, int bufSize)
{
    char* inBuf = (char*)inBuf_;
    const int TITLE_LEN = 64;
    if(bufSize<TITLE_LEN+(int)sizeof(tagDataLabel))
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
    if(bufSize<TITLE_LEN+(int)sizeof(tagDataLabel)+(int)sizeof(Complex)*matsize)
        return 0;

    matc* m = createMatC(plabel->dims[0], plabel->dims[1], plabel->dims[2],plabel->dims[3]);
    memcpy(m->data, pdata, sizeof(tagDataLabel)+sizeof(Complex)*matsize);
    memcpy(&label, plabel, sizeof (label));

    if(mat)
        delete[] mat;
    mat = m;

    return skip_size+TITLE_LEN+sizeof(tagDataLabel)+sizeof(Complex)*matsize;
}



// dim N sequence dim N : create a new mat dim N+1 as [2, ...]
UMatC& UMatC::sequence(UMatC& in)
{
    if(mat->dim_cnt>=4)
    {
        throw 1;
        return *this;
    }
    matc *newMat = createMatC(2, mat->dims[0], mat->dims[1], mat->dims[2]);
    int mat_size = getMatSize(mat);
    memcpy(newMat->data, mat->data, mat_size*sizeof (Complex));
    memcpy(newMat->data+mat_size, in.mat->data, mat_size*sizeof (Complex));
    freeMat(mat);
    mat = newMat;
    label.dims[3]=label.dims[2];
    label.dims[2]=label.dims[1];
    label.dims[1]=label.dims[0];
    label.dims[0]=2;
    return *this;
}
// dim N+1 append dim N : increase dim0 as [dim0+1, ...]
UMatC& UMatC::append(UMatC& in)
{
    int new_dim0 = mat->dims[0]+1;
    int old_mat_size = getMatSize(mat);
    if(new_dim0==20)
    {
        printf("ok\n");
    }
    resizeMat(mat, new_dim0);
    memcpy(mat->data+old_mat_size, in.mat->data, getMatSize(in.mat)*sizeof(Complex));
    label.dims[0] = new_dim0;
    return *this;
}
