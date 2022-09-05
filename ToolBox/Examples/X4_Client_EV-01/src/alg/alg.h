//
// Created by hwx on 2021/9/26.
//

#ifndef ESP32_ALG_H
#define ESP32_ALG_H

#define datatype double

typedef struct {
    datatype real;
    datatype imag;
}Complex;

typedef struct {
    int exist;
    int fastmove;
    datatype distance;
}x4result_t;

void findbreath(Complex* iqmat, int winlen, int fps, int bin, x4result_t* res);

#endif //ESP32_ALG_H
