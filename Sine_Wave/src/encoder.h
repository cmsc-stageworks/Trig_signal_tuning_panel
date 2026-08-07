#ifndef ENCODER_H
#define ENCODER_H

#include <arduino.h>
#include "waveform.h"
#include <Adafruit_seesaw.h>

#define ENCODER1_ADDR 0x36 //default I2C address of device
#define ENCODER2_ADDR 0x37 
#define ENCODER3_ADDR 0x38 

#define SS_SWITCH 24 //hardware value
#define SS_NEOPIX 6

// frequency, amplitude, phase shift
enum encoder {FREQ = 0, AMPL = 1, PHSH = 2};

void init_encoder();

void loop_encoder();

int32_t getBoundedEncoderPosition(Adafruit_seesaw *encoder);

waveform get_user_input();

uint16_t read_knob(Adafruit_seesaw *encoder);

uint16_t read_bounded_knob(encoder knob);

void override_user_input(waveform new_waveform);

#endif