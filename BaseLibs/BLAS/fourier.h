#ifndef _SENSOR_FORMULA_BASELIBS_BLAS_FFT_H
#define _SENSOR_FORMULA_BASELIBS_BLAS_FFT_H

#include "../UniversalData/UniversalData.h"

namespace sfblas
{
    int fft(UMat x, UMat y);

    int fft2d(UMat x, UMat y);

    int fftShift(Umat x, UMat y);
};

#endif