#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <Arduino.h>

struct waveform {
    uint16_t frequency; //1-16
    float amplitude; //0-1
    uint16_t phase_shift;
};

#endif