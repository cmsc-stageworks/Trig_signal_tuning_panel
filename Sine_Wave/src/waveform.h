#ifndef WAVEFORM_H
#define WAVEFORM_H

#include <Arduino.h>

struct waveform {
    uint16_t frequency;
    float amplitude;
    uint16_t phase_shift;
};

#endif