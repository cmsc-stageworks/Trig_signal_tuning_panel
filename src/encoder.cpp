#include "encoder.h"
#include <Adafruit_seesaw.h>

Adafruit_seesaw encoder_1;

void init_encoder() {
    Serial.begin(115200);
    if(!encoder_1.begin(ENCODER1_ADDR)) {
        Serial.println("can't connect to encoder 1");
    }
    encoder_1.pinMode(SS_SWITCH, INPUT_PULLUP);

    encoder_1.enableEncoderInterrupt();
}
//encoders are 4 times a click
void loop_encoder() {
    int32_t encoder_1_position = encoder_1.getEncoderPosition();
    Serial.println(encoder_1_position);
}

#define NUM_ENCODER_CLICKS_IN_RANGE 16
//encoders are 4 times a click
#define MAX_ENCODER_VALUE (NUM_ENCODER_CLICKS_IN_RANGE * 4)

int32_t getBoundedEncoderPosition(Adafruit_seesaw *encoder){
    int32_t pos = encoder->getEncoderPosition();
    int32_t unbounded_pos = pos;
    pos = (pos >= 0) ? pos : 0;
    pos = (pos <= MAX_ENCODER_VALUE) ? pos : MAX_ENCODER_VALUE;
    if (pos != unbounded_pos) {
        encoder->setEncoderPosition(pos);
    }
    return pos;
}
