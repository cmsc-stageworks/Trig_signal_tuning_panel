#ifndef TRIG_H
#define TRIG_H

#include "waveform.h"
#include <Adafruit_seesaw.h>


void init_trig();

void loop_trig();

void set_waveform_puzzle(waveform puzzle);
waveform get_waveform_puzzle();

void set_user_input(waveform puzzle); //used my main loop to record the user's actions
//override_user_input in encoder.h is used to override the user's actions
//get_user_input is also in encoder.h

#endif