#include "filter.h"
#include "blas_clang.h"

namespace sfblas
{


int smooth(UMatC& x, UMatC& y, int n/* = 5*/, int dim/* = DIM_INX_LAST_DIM*/)
{
    if(x.getMat()!=y.getMat())
        y = x; // create new matrix y

    if(dim==DIM_INX_LAST_DIM)
        dim = x.getLable()->dimCnt()-1;
    int len = x.getLable()->dims[dim];

    Complex * tmp_result = new Complex[len];

    typedef struct
    {
        Complex * tmp_result;
        int len;
        int n;
        int dim;
        UMatC* umat_y;
    }tagTmpFilter;
    tagTmpFilter t = {tmp_result, len, n, dim, &y};
    auto process_dim_data = [](Complex* dim_data, int i1, int i2, int i3, int i4, void* context)->int{
        tagTmpFilter *p = (tagTmpFilter*)context;
        smooth(dim_data, p->tmp_result, p->len, p->n);
        p->umat_y->updateDimData(p->tmp_result, p->dim, i1, i2, i3, i4);
        return 0;
    };
    x.getDimData(dim, process_dim_data, &t);

    delete[] tmp_result;


    return 0;
}

#include <stdio.h>
int fir_bandpass(UMatC& x, UMatC& y, int fps, UF l_pass_hz, UF h_pass_hz, int dim/* = DIM_INX_LAST_DIM*/)
{

    if(x.getMat()!=y.getMat())
        y = x; // create new matrix y

    if(dim==DIM_INX_LAST_DIM)
        dim = x.getLable()->dimCnt()-1;
    int len = x.getLable()->dims[dim];

    Complex * tmp_result = new Complex[len];

    typedef struct
    {
        Complex * tmp_result;
        int len;
        int fps;
        UF lp;
        UF hp;
        int dim;
        UMatC* umat_y;
    }tagTmpFilter;
    tagTmpFilter t = {tmp_result, len, fps, l_pass_hz, h_pass_hz, dim, &y};
    auto process_dim_data = [](Complex* dim_data, int i1, int i2, int i3, int i4, void* context)->int{
        tagTmpFilter *p = (tagTmpFilter*)context;
        //smooth(dim_data, p->tmp_result, p->len, p->n);
        fir_bandpass(dim_data, p->tmp_result, p->len, p->fps, p->lp, p->hp, 127);
        p->umat_y->updateDimData(p->tmp_result, p->dim, i1, i2, i3, i4);
        return 0;
    };
    x.getDimData(dim, process_dim_data, &t);

    delete[] tmp_result;


    return 0;
}







}