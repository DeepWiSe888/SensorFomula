#include "BaseDataDefine.h"

#include <memory.h>
#include <stdlib.h>

#ifndef taskMemAlloc
#define taskMemAlloc    malloc
#define taskMemFree     free
#endif


// how to: imaging a matrix, m at Y, n at X, so m*width+n
int dim2inx(int* dims, int m, int n) {return (m)*dims[1]+n;}
// how to: imaging a cube, m at Y, n at X, k at Z, so m*area(of X*Z) and n*Z and k
int dim3inx(int* dims, int m, int n, int k) {return (m)*dims[1]*dims[2]+(n)*dims[2]+k;}
// how to: imaging cubes at different spatio then you have 4d, m at spatio dim(passed through m cubes), then n/k/p as 3d above
int dim4inx(int* dims, int m, int n, int k, int p) {return (m)*dims[1]*dims[2]*dims[3]+(n)*dims[2]*dims[3]+(k)*dims[3]+p;}


matc* createMat1C(int M)
{
    complex * buf = (complex*)taskMemAlloc(M*sizeof(complex));
    bzero(buf, M*sizeof(complex));

    matc *mc = (matc*)taskMemAlloc(sizeof(matc));
    bzero(mc, sizeof(matc));

    mc->dim_cnt = 1;
    mc->dims[0] = M;
    mc->dims[1] = 0;
    mc->data = buf;

    return mc;
}
matc* createMat2C(int M, int N)
{
    complex * buf = (complex*)taskMemAlloc(M*N*sizeof(complex));
    bzero(buf, M*N*sizeof(complex));

    matc *mc = (matc*)taskMemAlloc(sizeof(matc));
    bzero(mc, sizeof(matc));

    mc->dim_cnt = 2;
    mc->dims[0] = M;
    mc->dims[1] = N;
    mc->data = buf;

    return mc;
}


matc* createMat3C(int M, int N, int P)
{
    complex * buf = (complex*)taskMemAlloc(M*N*P*sizeof(complex));
    bzero(buf, M*N*P*sizeof(complex));

    matc *mc = (matc*)taskMemAlloc(sizeof(matc));
    bzero(mc, sizeof(matc));

    mc->dim_cnt = 3;
    mc->dims[0] = M;
    mc->dims[1] = N;
    mc->dims[2] = P;
    mc->data = buf;

    return mc;
}

matc* createMat4C(int M, int N, int K, int P)
{
    complex * buf = (complex*)taskMemAlloc(M*N*K*P*sizeof(complex));
    bzero(buf, M*N*K*P*sizeof(complex));

    matc *mc = (matc*)taskMemAlloc(sizeof(matc));
    bzero(mc, sizeof(matc));

    mc->dim_cnt = 4;
    mc->dims[0] = M;
    mc->dims[1] = N;
    mc->dims[2] = K;
    mc->dims[3] = P;
    mc->data = buf;

    return mc;
}

// ignore P->K->N if is zero.
matc* createMatC(int M, int N, int K, int P)
{
    if(P)
        return createMat4C(M,N,K,P);
    else if(K)
        return createMat3C(M,N,K);
    else if(N)
        return createMat2C(M,N);
    else if(M)
        return createMat1C(M);
    else
        return 0;
}


// create a mat as same size
matc* createSameMat(void* _inMat)
{
    matc* inMat = (matc*)_inMat;
    matc *mc = (matc*)taskMemAlloc(sizeof(matc));
    if(mc==0)
        return 0;
    bzero(mc, sizeof(matc));
    memcpy(mc, inMat, (size_t)(&(((matc*)0)->data))); //copy size before data

    int bufsize = 1;
    int i;
    for(i=0;i<inMat->dim_cnt;i++)
        bufsize = bufsize * inMat->dims[i];

    complex * buf = (complex*)taskMemAlloc(bufsize*sizeof(complex));
    if(buf==0)
    {
        mc->data = 0;
        freeMat(mc);
        return 0;
    }
    bzero(buf, bufsize*sizeof(complex));
    mc->data = buf;
    return mc;
}

matc* copyMat(matc* m)
{
    matc* new_mat = createSameMat(m);
    if(new_mat==0)
        return 0;
    int datasize = getMatSize(m);
    memcpy(new_mat->data, m->data, datasize*sizeof (Complex));
    return new_mat;
}

matc * freeMat(matc* m)
{
    if(!m)
        return 0;
    taskMemFree(m->data);
    taskMemFree(m);
    m = 0;
    return m;
}

int getMatSize(void* _pMat)
{
    matc *pMat = (matc*)_pMat;
    int sz = 1;
    int i;
    for(i=0;i<pMat->dim_cnt;i++)
    {
        sz *= pMat->dims[i];
    }
    return sz;
}

