#include "encoder.h"
#include <Adafruit_seesaw.h>

Adafruit_seesaw encoders[3];
// frequency, amplitude, phase shift

int MAX_ENCODER_VALUES[3] = {16, 10, 16};

//Adafruit_seesaw frequency_encoder;
//Adafruit_seesaw amplitude_encoder;
//Adafruit_seesaw phase_shift_encoder;

void init_encoder() {
    Serial.begin(115200);
    if(!encoders[0].begin(ENCODER1_ADDR)) {
        Serial.println("can't connect to frequency encoder");
    }
    if(!encoders[1].begin(ENCODER2_ADDR)) {
        Serial.println("can't connect to amplitude encoder");
    }
    if(!encoders[2].begin(ENCODER3_ADDR)) {
        Serial.println("phase shift encoder not implemented yet");
        //Serial.println("can't connect to phase shift encoder");
    }
    encoders[0].pinMode(SS_SWITCH, INPUT_PULLUP);
    encoders[1].pinMode(SS_SWITCH, INPUT_PULLUP);
    encoders[2].pinMode(SS_SWITCH, INPUT_PULLUP);

    encoders[0].enableEncoderInterrupt();
    encoders[1].enableEncoderInterrupt();
    encoders[2].enableEncoderInterrupt();

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

//encoders are 4 times a click
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
    return {read_bounded_knob(0), read_bounded_knob(1)/10.0f, 0/*read_bounded_knob(2)-1 (zer-based)*/};
}

uint16_t read_knob(Adafruit_seesaw *encoder) {
    uint16_t knob_position = encoder->getEncoderPosition();
    return knob_position%16;
    //returns a position 0 to 15, looping
}

uint16_t read_bounded_knob(uint16_t knob) {
    //returns a bounded position for the given knob, dynamically assigning the max value
    uint16_t pos = encoders[knob].getEncoderPosition();
    //int32_t unbounded_pos = pos;
    pos = (pos >= 0) ? pos : 0;
    pos = (pos <= MAX_ENCODER_VALUES[knob]) ? pos : MAX_ENCODER_VALUES[knob];
    /*if (pos != unbounded_pos) {
        encoders[knob].setEncoderPosition(pos);
    }*/
    encoders[knob].setEncoderPosition(pos);
    return pos;
    //Is it really so computationally efficient to check that the positions differ, rather than blindly writing it every time anyway?
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
    encoders[0].setEncoderPosition(new_waveform.frequency * 4);
    encoders[1].setEncoderPosition(new_waveform.amplitude * 4);
    encoders[2].setEncoderPosition(new_waveform.phase_shift * 4);
}