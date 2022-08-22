#include "fourier.h"

#include <fftw3.h>
#include "BaseDataDefine.h"

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


int fft(UMatC& x, UMatC& y, uint32_t N=DIM_LENGTH, int dim = CALC_LAST_DIM)
{
    Complex * fft_x = nullptr;
    Complex * fft_y = nullptr;

    int dim_cnt = x.getLable()->dimCnt();
    if(dim_cnt<1)
        return -1;
    DataLabel  label_y = *x.getLable();
    int x_len = 0;
    if(dim==CALC_LAST_DIM||dim==dim_cnt-1)
    {
        x_len = x.getLable()->dims[dim_cnt-1];
        if(N==DIM_LENGTH||N==0)
            N=x_len; // N==DIM_LENGTH means N size as x;
    }
    else
    {
        return -2; //not support now
    }

    label_y.dims[dim_cnt-1] = N;
    y = UMatC(label_y);


    if(dim_cnt==1)
    {
        fft_x = x.At(0);
        fft_y = y.At(0);
        fft_c2c_1d(fft_x, x_len, fft_y, N);
    }
    else if(dim_cnt==2)
    {
        int i;
        for(i=0;i<label_y.dims[0];i++)
        {
            fft_x = x.At(i, 0);
            fft_y = y.At(i, 0);
            fft_c2c_1d(fft_x, x_len, fft_y, N);
        }
    }
    else if(dim_cnt==3)
    {
        int i, j;
        for(i=0;i<label_y.dims[0];i++)
        {
            for(j=0;j<label_y.dims[1];j++)
            {
                fft_x = x.At(i, j, 0);
                fft_y = y.At(i, j, 0);
                fft_c2c_1d(fft_x, x_len, fft_y, N);
            }
        }
    }
    else if(dim_cnt==4)
    {
        int i, j, k;
        for(i=0;i<label_y.dims[0];i++)
        {
            for(j=0;j<label_y.dims[1];j++)
            {
                for(k=0;k<label_y.dims[2];k++)
                {
                    fft_x = x.At(i, j, k, 0);
                    fft_y = y.At(i, j, k, 0);
                    fft_c2c_1d(fft_x, x_len, fft_y, N);
                }
            }
        }
    }

    return 0;
}

int fft2d(UMatC& x, UMatC& y, uint32_t N=DIM_LENGTH, int dim = CALC_LAST_DIM)
{
    return 0;
}

int fftShift(UMatC& x, UMatC& y, int dim = CALC_LAST_DIM)
{
    return 0;
}