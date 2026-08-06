#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <Arduino.h>

struct waveform {
    uint16_t frequency; //1-15
    uint16_t amplitude; //1-10
    uint16_t phase_shift; //0-15
};

#endif