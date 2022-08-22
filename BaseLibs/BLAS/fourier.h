#ifndef _SENSOR_FORMULA_BASELIBS_BLAS_FFT_H
#define _SENSOR_FORMULA_BASELIBS_BLAS_FFT_H

#include "../UniversalData/UniversalData.h"
#include <stdint.h>

#define     CALC_LAST_DIM   (99)
#define     DIM_LENGTH      (0)
namespace sfblas
{
    int fft(UMatC& x, UMatC& y, uint32_t N=DIM_LENGTH, int dim = CALC_LAST_DIM);

    int fft2d(UMatC& x, UMatC& y, uint32_t N=DIM_LENGTH, int dim = CALC_LAST_DIM);

    int fftShift(UMatC& x, UMatC& y, int dim = CALC_LAST_DIM);
};

#endif