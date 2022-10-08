#ifndef INC_BASE_DATA_DEFINE_H_
#define INC_BASE_DATA_DEFINE_H_

//#ifdef __cplusplus
//extern "C"{
//#endif

//#define DATATYPE_DOUBLE
#define DATATYPE_FLOAT

#ifdef DATATYPE_FLOAT
typedef float UF;              // UF: universal float
#endif
#ifdef DATATYPE_DOUBLE
typedef double UF;              // UF: universal float
#endif

#define        ZERO_TRESH   (1e-6)

#pragma pack(1)

/// ------- base data type defines ------- ///

typedef struct _complex
{
    union
    {
        UF i;
        UF real;
    };
    union
    {
        UF q;
        UF imag;
    };
#ifdef __cplusplus

    _complex()
    {
        i = 0;
        q = 0;
    }

    _complex(UF i_)
    {
        i = i_;
        q = 0;
    }

    _complex(UF i_, UF q_)
    {
        i = i_;
        q = q_;
    }

    operator double() const
    {
        return i;
    }

    operator float() const
    {
        return i;
    }

    _complex operator+(_complex& a)
    {
        return _complex(i+a.i, q+a.q);
    }

    _complex operator-(_complex& a)
    {
        return _complex(i-a.i, q-a.q);
    }

    _complex operator*(_complex& a)
    {
        //(x+iy)(a+ib) = xa-yb+ixb+iay
        return _complex(i*a.i-q*a.q, i*a.q+q*a.i);
    }

    //UF abs()
    //{
    //    return sqrt(i*i+q*q);
    //}
#endif  //__cplusplus

} complex, Complex;


/// reserved for c style
// +
void complex_add(Complex*a, Complex*b, Complex* out);
// -
void complex_sub(Complex*a, Complex*b, Complex* out);
// *
void complex_mul(Complex*a, Complex*b, Complex* out);

void complex_mul_expjx(Complex*a, UF x, Complex* out);

UF complex_abs(Complex* a);
UF complex_angle(Complex* a);

UF absC2d(Complex c);

__inline UF min(UF d1, UF d2){return d1<d2?d1:d2;}
__inline UF max(UF d1, UF d2){return d1>d2?d1:d2;}









/// --------------- Matrix Definitions ----------------- ///

//matrix complex
#define MAX_DIM_CNT    (5)
typedef struct _mat_c
{
    int dim_cnt;
    int dims[MAX_DIM_CNT];
    int max_memory_size;
    complex *data;    // use like this matc.data[dim3inx(i,j,k)]
} matc;


//matrix real float
typedef struct _mat_r
{
    int dim_cnt;
    int dims[MAX_DIM_CNT];
    int max_memory_size;
    UF *data;
} matr;
#pragma pack()


int dim2inx(int *dims, int m, int n);

int dim3inx(int *dims, int m, int n, int k);

int dim4inx(int *dims, int m, int n, int k, int p);

// mat 1/2/3/4 dims value
#define M1V(m, i)           (m->data[i])
#define M2V(m, i, j)        (m->data[dim2inx(m->dims, i,j)])
#define M3V(m, i, j, k)     (m->data[dim3inx(m->dims, i,j,k)])
#define M4V(m, i, j, k, p)  (m->data[dim4inx(m->dims, i,j,k,p)])


matc *createMat1C(int M);

//m*n like : frames*range_bins
matc *createMat2C(int M, int N);

//m*n*k
matc *createMat3C(int M, int N, int K);

//m*n*k
matc *createMat4C(int M, int N, int K, int P);

// ignore P->K->N if is zero.
matc *createMatC(int M, int N, int K, int P);

// create a mat as same size
matc *createSameMat(void *inMat);

// resize mat dims
int resizeMat(matc* m, int newM/*, int newN, int newK, int newP*/);

matc *copyMat(matc *m);

// always return NULL, for graceful coding : m = freeMat(m);
matc *freeMat(matc *m);


int getMatSize(void *pMat);

//int copyDimData(matc* m, complex* out, int dim_inx);

//test
void printMat(matc *mc);

//#ifdef __cplusplus
//}
//#endif //__cplusplus

#endif