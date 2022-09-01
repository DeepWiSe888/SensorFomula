#include "fourier.h"

#include "BaseDataDefine.h"
#include "blas_clang.h"

namespace sfblas
{

int fft(UMatC& x, UMatC& y, uint32_t N/*=DIM_LENGTH*/, int dim/* = CALC_LAST_DIM*/)
{
    Complex * fft_x = nullptr;
    Complex * fft_y = nullptr;

    int dim_cnt = x.getLable()->dimCnt();
    if(dim_cnt<1)
        return -1;
    DataLabel  label_y = *x.getLable();
    int x_len = 0;
    if(dim==DIM_INX_LAST_DIM||dim==dim_cnt-1)
        dim = dim_cnt-1;

    x_len = x.getLable()->dims[dim];
    if(N==SIZE_AS_DIM_LENGTH||N==0)
        N=x_len; // N==DIM_LENGTH means N size as x;

    label_y.dims[dim] = N;
    y = UMatC(label_y);

    fft_x = new Complex[x_len];
    fft_y = new Complex[N];

    typedef struct _tmp_fft
    {
        int dim;
        Complex * x;
        Complex * y;
        int len;
        uint32_t N;
        UMatC* umat_y;
    }tagTmpFFT;
    tagTmpFFT t = {dim,fft_x, fft_y, x_len, N, &y};


    //auto process_dim_data_capture = [dim, fft_x, x_len, fft_y, N, &y](Complex* dim_data, int i1, int i2, int i3, int i4)->int{
    //    fft_c2c_1d(fft_x, x_len, fft_y, N);
    //    y.updateDimData(fft_y, dim, i1, i2, i3, i4);
    //    return 0;
    //};
    auto process_dim_data = [](Complex* dim_data, int i1, int i2, int i3, int i4, void* context)->int{
        tagTmpFFT *p = (tagTmpFFT*)context;
        fft_c2c_1d(dim_data, p->len, p->y, p->N);
        p->umat_y->updateDimData(p->y, p->dim, i1, i2, i3, i4);
        return 0;
    };
    x.getDimData(dim, process_dim_data, &t);


    delete[] fft_x;
    delete[] fft_y;

    return 0;

}


int fft2d(UMatC& x, UMatC& y, uint32_t N/*=DIM_LENGTH*/, int dim/* = CALC_LAST_DIM*/)
{
    return 0;
}

int fftShift(UMatC& x, UMatC& y, int dim/* = CALC_LAST_DIM*/)
{
    return 0;
}


};