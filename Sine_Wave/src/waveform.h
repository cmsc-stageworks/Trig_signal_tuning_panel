#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <Arduino.h>

struct waveform {
    uint8_t frequency; //1-15
    uint8_t amplitude; //1-10
    uint8_t phase_shift; //0-15
    uint8_t noise_level; //0-8 corresponding to pixels of noise
};

#endif