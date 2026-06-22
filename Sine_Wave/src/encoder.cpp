#include "encoder.h"
#include <Adafruit_seesaw.h>

Adafruit_seesaw frequency_encoder;
Adafruit_seesaw amplitude_encoder;
Adafruit_seesaw phase_shift_encoder;

void init_encoder() {
    Serial.begin(115200);
    if(!frequency_encoder.begin(ENCODER1_ADDR)) {
        Serial.println("can't connect to frequency encoder");
    }
    if(!amplitude_encoder.begin(ENCODER2_ADDR)) {
        Serial.println("can't connect to amplitude encoder");
    }
    if(!phase_shift_encoder.begin(ENCODER3_ADDR)) {
        Serial.println("can't connect to phase shift encoder");
    }
    frequency_encoder.pinMode(SS_SWITCH, INPUT_PULLUP);
    amplitude_encoder.pinMode(SS_SWITCH, INPUT_PULLUP);
    phase_shift_encoder.pinMode(SS_SWITCH, INPUT_PULLUP);

    frequency_encoder.enableEncoderInterrupt();
    amplitude_encoder.enableEncoderInterrupt();
    phase_shift_encoder.enableEncoderInterrupt();

    Serial.println("encoders initialized");
}
//encoders are 4 times a click
void loop_encoder() {
    return;
}

#define NUM_ENCODER_CLICKS_IN_RANGE 16
//encoders are 4 times a click
#define MAX_ENCODER_VALUE (NUM_ENCODER_CLICKS_IN_RANGE * 4)

int32_t getBoundedEncoderPosition(Adafruit_seesaw *encoder){ //returns an int from 0 to 16
    int32_t pos = encoder->getEncoderPosition();
    int32_t unbounded_pos = pos;
    pos = (pos >= 0) ? pos : 0;
    pos = (pos <= MAX_ENCODER_VALUE) ? pos : MAX_ENCODER_VALUE;
    if (pos != unbounded_pos) {
        encoder->setEncoderPosition(pos);
    }
    return pos / 4;
    //probably need to remove /4 to make every click change the waveform
}

waveform get_user_input() {
    // return {getBoundedEncoderPosition(&frequency_encoder), getBoundedEncoderPosition(&amplitude_encoder), getBoundedEncoderPosition(&phase_shift_encoder)};
    return {getBoundedEncoderPosition(&frequency_encoder), 0.5, 0};

}

void override_user_input(waveform new_waveform) {
    frequency_encoder.setEncoderPosition(new_waveform.frequency * 4);
    amplitude_encoder.setEncoderPosition(new_waveform.amplitude * 4);
    phase_shift_encoder.setEncoderPosition(new_waveform.phase_shift * 4);
}