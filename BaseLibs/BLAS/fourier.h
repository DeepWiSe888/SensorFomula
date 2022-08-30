#ifndef _SENSOR_FORMULA_BASELIBS_BLAS_FFT_H
#define _SENSOR_FORMULA_BASELIBS_BLAS_FFT_H

#include "../UniversalData/UniversalData.h"
#include "blas_base_define.h"

namespace sfblas
{
    int fft(UMatC& x, UMatC& y, uint32_t N=SIZE_AS_DIM_LENGTH, int dim = DIM_INX_LAST_DIM);

    int fft2d(UMatC& x, UMatC& y, uint32_t N=SIZE_AS_DIM_LENGTH, int dim = DIM_INX_LAST_DIM);

    int fftShift(UMatC& x, UMatC& y, int dim = DIM_INX_LAST_DIM);
};

#endif