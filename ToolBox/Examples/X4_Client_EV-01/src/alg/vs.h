
#ifndef ESP32_VS_H
#define ESP32_VS_H

#define DEFAULT_FPS     (40)


void vs_init(int fps);

void vs_add_frame(float* fdata, int cnt);



#endif