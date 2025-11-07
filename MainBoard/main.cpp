#include <MainBoard.h>
#include <Arduino.h>

MainBoard board;

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing the trig signal pannel");

    board.begin();
}

void loop( {
    board.update();
    delay(10);
})
