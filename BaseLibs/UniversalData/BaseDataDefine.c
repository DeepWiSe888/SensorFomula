#include "BaseDataDefine.h"

#include <memory.h>


// how to: imaging a matrix, m at Y, n at X, so m*width+n
int dim2inx(int* dims, int m, int n) {return (m)*dims[1]+n;}
// how to: imaging a cube, m at Y, n at X, k at Z, so m*area(of X*Z) and n*Z and k
int dim3inx(int* dims, int m, int n, int k) {return (m)*dims[1]*dims[2]+(n)*dims[2]+k;}
// how to: imaging cubes at different spatio then you have 4d, m at spatio dim(passed through m cubes), then n/k/p as 3d above
int dim4inx(int* dims, int m, int n, int k, int p) {return (m)*dims[1]*dims[2]*dims[3]+(n)*dims[2]*dims[3]+(k)*dims[3]+p;}


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


// create a mat as same size
matc* createSameMat(void* _inMat)
{
    matc* inMat = (matc*)_inMat;
    matc *mc = (matc*)taskMemAlloc(sizeof(matc));
    bzero(mc, sizeof(matc));
    memcpy(mc, inMat, (size_t)(&(((matc*)0)->data))); //copy size before data

    int bufsize = 1;
    int i;
    for(i=0;i<inMat->dim_cnt;i++)
        bufsize = bufsize * inMat->dims[i];

    complex * buf = (complex*)taskMemAlloc(bufsize*sizeof(complex));
    bzero(buf, bufsize*sizeof(complex));
    mc->data = buf;
    return mc;
}

int getMatSize(void* pMat)
{
    return sizeof(matc);
}
