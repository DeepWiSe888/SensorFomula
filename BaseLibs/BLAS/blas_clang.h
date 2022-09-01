#ifndef _SENSOR_FORMULA_BASELIBS_BLAS_FILTER_CLANG_H
#define _SENSOR_FORMULA_BASELIBS_BLAS_FILTER_CLANG_H

#include "BaseDataDefine.h"


#ifdef __cplusplus
extern "C"{
#endif


/// -------------------- filters -------------------- //

int smooth(Complex* x, Complex* y, int len, int n);

int fir_bandpass(Complex* x, Complex* y, int len, int fps, UF l_pass_hz, UF h_pass_hz, int order);





/// -------------------- fourier transforms -------------------- //

/// fft c2c
int fft_c2c_1d(Complex * x, int x_len, Complex * y, int n);

///fft3w real to complex
int fftr2c(int n,UF *in, int len, Complex *result);

///fft3w complex to real
int ifftc2r(int N,Complex *in,int len, UF *out);

///frequency domain convolute
void fftconv(int xlen,UF *x,int klen, UF *k, UF *y);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif