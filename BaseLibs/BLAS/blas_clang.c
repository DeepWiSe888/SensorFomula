#include "blas_clang.h"
#include <stdlib.h>
#include <math.h>

#include <fftw3.h>

#ifndef pi
#define pi (3.14159265)
#endif

#ifndef nullptr
#define nullptr (0)
#endif


int smooth(Complex* x, Complex* y, int size, int width)
{
    width = width - 1 + (width & 0x01);
    int side = width / 2;
    if (width <= 1 || side <= 0) {
        return 0;
    }

    for (int i = 1; i < size -1; i++) {
        Complex* left = nullptr;
        int calcs = 1;
        if (i < side) {
            //  左边不足
            left = x;
            calcs = i + i + 1;
        }
        else if (size -i -1 < side) {
            // 右边数据不足
            left = x + i - (size - i - 1);
            calcs = (size - i - 1) * 2 + 1;
        }
        else {
            left = x + i - side;
            calcs = width;
        }

        y[i].real = 0.;
        y[i].imag = 0.;
        for (int j = 0; j < calcs; j++, left++) {
            y[i].real += left->real;
            y[i].imag += left->imag;
        }
        y[i].real /= calcs;
        y[i].imag /= calcs;
    }
    y[0] = x[0];
    y[size - 1] = x[size - 1];

    return 0;
}





///sinc
UF sinc(UF x){
    if(x < 1e-9)
        return 1.0;
    else
        return sin(x)/x;
}

///firwin
void firwin(int numtaps,UF fp,UF fs, UF *h){
    UF alpha = 0.5 * (numtaps - 1);
    UF scale_frequency = 0;
    if(fp == 0){
        scale_frequency = 0.0;
    }else if(fs == 1){
        scale_frequency = 1.0;
    }else{
        scale_frequency = 0.5 * (fp + fs);
    }
    UF s = 0;
    UF *temp = (UF *)malloc(sizeof(UF)*numtaps);
    for(int i = 0;i < numtaps;i++){
        UF m = (i - alpha);
        UF hamming = 0.54 - 0.46*cos(2*pi*i/((UF)numtaps - 1.0));
        UF x1 = fs * m * pi;
        UF x2 = fp * m * pi;

        UF tt = 0.0;
        tt = tt + fs*sinc(x1);
        tt = tt - fp*sinc(x2);
        tt = tt * hamming;
        UF c = cos(pi * m * scale_frequency);
        s += tt * c;
        temp[i] = tt;
    }
    for(int i = 0;i < numtaps;i++){
        h[i] = temp[i] / s;
    }
    free(temp);
}

int fir_bandpass(Complex* x, Complex* y, int len, int fps, UF l_pass_hz, UF h_pass_hz, int order)
{
    UF npyfp = l_pass_hz /(fps / 2.0);
    UF npyfs = h_pass_hz /(fps / 2.0);
    UF *k = (UF*)malloc(sizeof(UF)*order);
    firwin(order,npyfp,npyfs, k);
    UF *xreal = (UF*)malloc(sizeof(UF)*len);
    UF *ximag = (UF*)malloc(sizeof(UF)*len);
    for(int i = 0;i < len;i++){
        xreal[i] = x[i].real;
        ximag[i] = x[i].imag;
    }
    UF *xr = (UF*)malloc(sizeof(UF)*len);
    fftconv(len,xreal,order,k,xr);
    UF *xi = (UF*)malloc(sizeof(UF)*len);
    fftconv(len,ximag,order,k,xi);
    for(int i = 0;i < len;i++){
        y[i].real = xr[i];
        y[i].imag = xi[i];
    }
    free(k);
    free(xreal);
    free(ximag);
    free(xr);
    free(xi);
    return 0;
}








/// fft c2c
int fft_c2c_1d(Complex * x, int x_len, Complex * y, int n)
{
    fftw_complex * fft_x = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*n);
    fftw_complex * fft_y = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*n);
    int i;
    for(i=0;i<n;i++)
    {
        if(i>=x_len)
        {// zero padding
            fft_x[i][0] = 0;
            fft_x[i][1] = 0;
        }
        else
        {
            fft_x[i][0] = x[i].i;
            fft_x[i][1] = x[i].q;
        }
    }
    fftw_plan p = fftw_plan_dft_1d(n, fft_x, fft_y, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p);

    for(i=0;i<n;i++)
    {
        y[i].i = fft_y[i][0];
        y[i].q = fft_y[i][1];
    }

    fftw_free(fft_x);
    fftw_free(fft_y);

    return 0;

}


///fft3w real to complex
int fftr2c(int n,UF *in, int len, Complex *result)
{
    //pthread_mutex_lock(&fftw_mutex);
    double *indata = (double*)malloc(sizeof(double)*n);
    for(int i = 0;i < n;i++)
    {
        if(i < len)
        {
            indata[i] = in[i];
        }
        else
        {
            indata[i] = 0;
        }
    }

    //if(sizeof(UF) == sizeof(double))
#ifdef DATATYPE_DOUBLE
    {
        fftw_complex* out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
        fftw_plan p = fftw_plan_dft_r2c_1d(n,indata,out,FFTW_ESTIMATE);
        fftw_execute (p);
        fftw_destroy_plan(p);
        for(int i = 0;i < n;i++)
        {
            result[i].real = out[i][0];
            result[i].imag = out[i][1];
        }
        fftw_free(out);
    }
    //else if(sizeof(UF) == sizeof(float))
#else
    {
        fftw_complex* out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
        fftw_plan p = fftw_plan_dft_r2c_1d(n,indata,out,FFTW_ESTIMATE);
        fftw_execute (p);
        fftw_destroy_plan(p);
        for(int i = 0;i < n;i++)
        {
            result[i].real = out[i][0];
            result[i].imag = out[i][1];
        }
        fftw_free(out);
    }
#endif
    free(indata);
    //pthread_mutex_unlock(&fftw_mutex);
    return 0;
}

///fft3w complex to real
int ifftc2r(int N,Complex *in,int len, UF *out)
{
    //pthread_mutex_lock(&fftw_mutex);

    //if(sizeof(UF) == sizeof(double))
#ifdef DATATYPE_DOUBLE
    {
        fftw_complex* cply = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
        for(int i=0;i<N;i++)
        {
            if(i < len)
            {
                cply[i][0] = in[i].real;
                cply[i][1] = in[i].imag;
            }
            else
            {
                cply[i][0] = 0;
                cply[i][1] = 0;
            }
        }
        fftw_plan p =fftw_plan_dft_c2r_1d(N, cply, out,FFTW_ESTIMATE);
        fftw_execute(p);
        fftw_destroy_plan(p);
        fftw_free(cply);
    }
    //else if(sizeof(UF) == sizeof(float))
#else
    {
        double* out_double = (double *) malloc(sizeof (double )*N);
        fftw_complex* cply = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
        for(int i=0;i<N;i++)
        {
            if(i < len)
            {
                cply[i][0] = in[i].real;
                cply[i][1] = in[i].imag;
            }
            else
            {
                cply[i][0] = 0;
                cply[i][1] = 0;
            }
        }
        fftw_plan p =fftw_plan_dft_c2r_1d(N, cply, out_double,FFTW_ESTIMATE);
        for(int i=0;i<N;i++)
            out[i] = out_double[i];
        fftw_execute(p);
        fftw_destroy_plan(p);
        fftw_free(cply);
        free(out_double);
    }
#endif

    //pthread_mutex_unlock(&fftw_mutex);
    return 0;
}


void fftconv(int xlen,UF *x,int klen, UF *k, UF *y){
    int s1 = xlen;
    int s2 = klen;
    int shape = s1  + s2 - 1;

    //fast fft len
    int nfft = shape;

    Complex *xfft = (Complex*)malloc(sizeof(Complex) * nfft);
    fftr2c(nfft,x,s1,xfft);

    Complex *kfft = (Complex*)malloc(sizeof(Complex) * nfft);
    fftr2c(nfft,k,s2,kfft);

    int fshap =  nfft / 2 + 1;
    Complex *mul =(Complex*)malloc(sizeof(Complex)* fshap);
    for(int i = 0;i < fshap;i++){
        Complex vtmp = xfft[i];
        Complex ktmp = kfft[i];
        Complex multmp;
        multmp.real = 0;
        multmp.imag = 0;
        complex_mul(&vtmp,&ktmp,&multmp);
        mul[i].real = multmp.real;
        mul[i].imag = multmp.imag;
    }
    free(xfft);
    free(kfft);
    UF *res =(UF*)malloc(sizeof(UF) * nfft);
    ifftc2r(nfft,mul,fshap,res);
    free(mul);

    for(int i = 0;i < nfft;i++){
        res[i] = res[i] / nfft;
    }
    int startind = (shape - s1) / 2;
    int endind = startind + s1;
    for(int i = startind;i < endind;i++){
        y[i - startind] = res[i];
    }
    free(res);
}