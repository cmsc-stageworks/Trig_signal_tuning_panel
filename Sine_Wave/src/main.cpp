#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include <MainBoard.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_ST7796S.h>
#include <Wire.h>
#include <Ethernet3.h>
#include <SdFat.h>
#include "trig.h"
#include "encoder.h"

extern SPIClass mainBoardSpi;

void setup() {

  MainBoardStart(false);
  init_trig();
  init_encoder();
  waveform puzzle = {5, 0.5f, 0};
  set_waveform_puzzle(puzzle);
  Serial.println("setup complete");
}

void loop() {
  set_user_input(get_user_input());
  loop_trig();
  loop_encoder();
  delay(10);
}
