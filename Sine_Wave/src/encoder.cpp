#include "encoder.h"
#include <Adafruit_seesaw.h>

Adafruit_seesaw encoders[3];

//these are from waveform.h
int MAX_ENCODER_VALUES[3] = {15, 10, 15};
int MAX_ENCODER_OVERRIDES[3] = {15, 10, 0};
int MIN_ENCODER_VALUES[3] = {1, 1, 0};
int MIN_ENCODER_OVERRIDES[3] = {1, 1, 15};
//phase shift encoder wraps around

//Adafruit_seesaw frequency_encoder;
//Adafruit_seesaw amplitude_encoder;
//Adafruit_seesaw phase_shift_encoder;

void init_encoder() {
    Serial.begin(115200);
    if(!encoders[FREQ].begin(ENCODER1_ADDR)) {
        Serial.println("can't connect to frequency encoder");
    }
    if(!encoders[AMPL].begin(ENCODER2_ADDR)) {
        Serial.println("can't connect to amplitude encoder");
    }
    if(!encoders[PHSH].begin(ENCODER3_ADDR)) {
        Serial.println("can't connect to phase shift encoder");
    }
    encoders[FREQ].pinMode(SS_SWITCH, INPUT_PULLUP);
    encoders[AMPL].pinMode(SS_SWITCH, INPUT_PULLUP);
    encoders[PHSH].pinMode(SS_SWITCH, INPUT_PULLUP);

    encoders[FREQ].enableEncoderInterrupt();
    encoders[AMPL].enableEncoderInterrupt();
    encoders[PHSH].enableEncoderInterrupt();

    /*This would be a really nice for loop if that doesn't break anything
    for (int i = 0; i < 3; i++) {
        if(!encoders[i].begin(ENCODER1_ADDR)) {
            if(i==0){
                Serial.println("can't connect to frequency encoder");
            } else if(i==1){
                Serial.println("can't connect to amplitude encoder");
            } else if(i==2){
                Serial.println("can't connect to phase shift encoder");
            }
        }
        encoders[i].pinMode(SS_SWITCH, INPUT_PULLUP);
        encoders[i].enableEncoderInterrupt();
    } */

    Serial.println("encoders initialized");
}

void loop_encoder() {
    //needs...something
    return;
}

/*#define MAX_ENCODER_VALUE 15

//Hard-coded to return frequency bounds
int32_t getBoundedEncoderPosition(Adafruit_seesaw *encoder){ //returns an int from 0 to 16
    int32_t pos = encoder->getEncoderPosition();
    int32_t unbounded_pos = pos;
    pos = (pos >= 0) ? pos : 0;
    pos = (pos <= MAX_ENCODER_VALUE) ? pos : MAX_ENCODER_VALUE;
    if (pos != unbounded_pos) {
        encoder->setEncoderPosition(pos);
    }
    return pos;
}*/

waveform get_user_input() {
    // return {getBoundedEncoderPosition(&frequency_encoder), getBoundedEncoderPosition(&amplitude_encoder), getBoundedEncoderPosition(&phase_shift_encoder)};
    //return {getBoundedEncoderPosition(&frequency_encoder), getBoundedEncoderPosition(&amplitude_encoder), 0};
    return {read_bounded_knob(FREQ), read_bounded_knob(AMPL), read_bounded_knob(PHSH)};
}

// uint16_t read_knob(Adafruit_seesaw *encoder) {
//     uint16_t knob_position = encoder->getEncoderPosition();
//     return knob_position%16;
//     //returns a position 0 to 15, looping
// }

uint16_t read_bounded_knob(encoder encoder) {
    //returns a bounded position for the given knob, dynamically assigning the max value
    int32_t pos = encoders[encoder].getEncoderPosition();
    int32_t unbounded_pos = pos;
    pos = (pos >= MIN_ENCODER_VALUES[encoder]) ? pos : MIN_ENCODER_OVERRIDES[encoder];
    pos = (pos <= MAX_ENCODER_VALUES[encoder]) ? pos : MAX_ENCODER_OVERRIDES[encoder];
    if (pos != unbounded_pos) {
        encoders[encoder].setEncoderPosition(pos);
    }
    return pos;
    //Is it really so computationally efficient to check that the positions differ, rather than blindly writing it every time anyway?
    //Yes, because most of the time the encoder value won't have changed between calls and I2C writes take a long time compared to CPU cycles.
    //There'd be no point in avoiding the writes if this wasn't an external bus
}

/*int32_t read_knob() { //you could hard-code three methods for the knobs, but I like the array solution
    int32_t knob_position = amplitude_encoder.getEncoderPosition();
    return knob_position;
}*/
/*
    switch (read_knob()){
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        default:
            
    }
*/

void override_user_input(waveform new_waveform) {
    encoders[FREQ].setEncoderPosition(new_waveform.frequency);
    encoders[AMPL].setEncoderPosition(new_waveform.amplitude);
    encoders[PHSH].setEncoderPosition(new_waveform.phase_shift);
}