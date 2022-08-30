#ifndef _SENSOR_FORMULA_BASELIBS_BLAS_FILTER_H
#define _SENSOR_FORMULA_BASELIBS_BLAS_FILTER_H

#include "../UniversalData/UniversalData.h"
#include "blas_base_define.h"

namespace sfblas
{
    int smooth(UMatC& x, UMatC& y, int n = 5, int dim = DIM_INX_LAST_DIM);
    int fir_bandpass(UMatC& x, UMatC& y, int fps, UF l_pass_hz, UF h_pass_hz, int dim = DIM_INX_LAST_DIM);

};

#endif