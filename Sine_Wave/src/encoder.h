#ifndef ENCODER_H
#define ENCODER_H

#include <arduino.h>
#include "waveform.h"

#define ENCODER1_ADDR 0x36 //default I2C address of device
#define ENCODER2_ADDR 0x37 //default I2C address of device
#define ENCODER3_ADDR 0x38 //default I2C address of device

#define SS_SWITCH 24 //hardware value
#define SS_NEOPIX 6

void init_encoder();

void loop_encoder();

int32_t getBoundedEncoderPosition(Adafruit_seesaw *encoder);

waveform get_user_input();

void override_user_input(waveform new_waveform);

#endif