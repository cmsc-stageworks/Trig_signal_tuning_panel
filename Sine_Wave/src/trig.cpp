#include <Adafruit_ILI9341.h>
#include <Adafruit_ST7796S.h>
#include "trig.h"
#include "MainBoardPinout.h"
#include "MainBoard.h"
#include <math.h>
#include <Adafruit_seesaw.h>
#include <Arduino.h>
#include "global_flags.h"

extern SPIClass mainBoardSpi;

static Adafruit_ST7796S screen = Adafruit_ST7796S(&mainBoardSpi, MAIN_BOARD_LCD_1_CS, MAIN_BOARD_LCD_DC, MAIN_BOARD_LCD_RESET);

static waveform target_puzzle;
static waveform current_attempt;
static boolean has_current_puzzle;
void check_puzzle_completion();
bool check_puzzle_update();
float last_amplitude;
float last_frequency;
float last_phase_shift;


// moved read_knob to encoder.cpp

float normalize(uint16_t x) {
    float MIN_NUM_HORIZ_PERIODS_IN_WIDTH = 0.8;
    uint16_t w = screen.width();
    float x_fractional = (x - (w/2)) / (float)w;
    return x_fractional * MIN_NUM_HORIZ_PERIODS_IN_WIDTH * 2 * PI;
}

uint16_t denormalize(float y) { //y goes from -1 to 1
    float MAX_AMPLITUDE_AS_FRACTION_OF_HEIGHT = 0.7;
    uint16_t h = screen.height();
    float y_as_float = y * h * MAX_AMPLITUDE_AS_FRACTION_OF_HEIGHT / 2; //divide by 2 because the input has range 2
    uint16_t output =  (h/2) + (float)y_as_float;
    return output;
}

static const uint8_t frequencyConverter[16] = {0,1,2,3,4,5,6,7,8,9,10,12,14,16,18,20};

void draw_trig(Adafruit_ST7796S *screen, waveform target, waveform attempt, waveform old_target, waveform old_attempt) {
    static uint16_t phase_offset = 0;
    #define phase_offset_delta SCANSPEED
    int h = screen->height();
    int w = screen->width();
    //draw waveforms
    uint16_t old_last_target_y = 0;
    uint16_t old_last_attempt_y = 0;
    uint16_t last_target_y = 0;
    uint16_t last_attempt_y = 0;
    uint16_t old_current_target_y = denormalize(old_target.amplitude * sin(frequencyConverter[old_target.frequency] * ((float) old_target.phase_shift + phase_offset + normalize(0))));
    uint16_t old_current_attempt_y = denormalize(old_attempt.amplitude * sin(frequencyConverter[old_attempt.frequency] * ((float) old_attempt.phase_shift + phase_offset + normalize(0))));
    uint16_t current_target_y = denormalize(target.amplitude * sin(frequencyConverter[target.frequency] * ((float) target.phase_shift + phase_offset + normalize(0) + phase_offset_delta)));
    uint16_t current_attempt_y = denormalize(attempt.amplitude * sin(frequencyConverter[attempt.frequency] * ((float) attempt.phase_shift + phase_offset + normalize(0) + phase_offset_delta)));
    screen->fillCircle(0, old_current_attempt_y, 2, 0);
    screen->fillCircle(0, old_current_target_y, 2, 0);
    screen->fillCircle(0, current_attempt_y, 2, screen->color565(242, 74, 98));
    screen->fillCircle(0, current_target_y, 2, screen->color565(74, 242, 98));
    for(int x=1; x<w; x++){ //skip the first one because we just took care of it
        old_last_attempt_y = old_current_attempt_y;
        last_attempt_y = current_attempt_y;
        old_current_attempt_y = denormalize(old_attempt.amplitude * sin(frequencyConverter[old_attempt.frequency] * ((float) old_attempt.phase_shift + phase_offset + normalize(x))));
        current_attempt_y = denormalize(attempt.amplitude * sin(frequencyConverter[attempt.frequency] * ((float) attempt.phase_shift + phase_offset + normalize(x) + phase_offset_delta)));
        old_last_target_y = old_current_target_y;
        last_target_y = current_target_y;
        old_current_target_y = denormalize(old_target.amplitude * sin(frequencyConverter[old_target.frequency] * ((float) old_target.phase_shift + phase_offset + normalize(x))));
        current_target_y = denormalize(target.amplitude * sin(frequencyConverter[target.frequency] * ((float) target.phase_shift + phase_offset + normalize(x) + phase_offset_delta)));
        screen->drawLine(x-1, old_last_target_y, x, old_current_target_y, 0); //draw over the last line
        if (((x/(old_attempt.frequency >= 8 ? 1 : 3)) % 2) == 0) {
            screen->drawLine(x-1, old_last_attempt_y, x, old_current_attempt_y, 0);
        }
        screen->drawLine(x-1, last_target_y, x, current_target_y, screen->color565(74, 242, 98));
        if (((x/(attempt.frequency >= 8 ? 1 : 3)) % 2) == 0) {
            screen->drawLine(x-1, last_attempt_y, x, current_attempt_y, screen->color565(242, 74, 98));
        }
    }
    phase_offset = (phase_offset + 1) % 5040;
}

void init_trig() {
    pinMode(MAIN_BOARD_LCD_RESET, OUTPUT);
    digitalWrite(MAIN_BOARD_LCD_RESET, HIGH);
    screen.init(320, 480, 0, 0, ST7796S_BGR);
    screen.setRotation(1); //width is the long way
    screen.invertDisplay(true);
    screen.fillScreen(screen.color565(0, 255, 255));
    Serial.println("GFX initialized");
    screen.fillScreen(screen.color565(0,0,0));
}

void loop_trig() {
    Serial.println("in trig loop");
    if (has_current_puzzle&&check_puzzle_update()) { 
        //check_puzzle_update is now deprecated because the puzzle now updates every frame, due to scrolling
        static waveform old_target = target_puzzle, last_attempt = current_attempt;
        draw_trig(&screen, target_puzzle, current_attempt, old_target, last_attempt);
        old_target = target_puzzle;
        last_attempt = current_attempt;
        check_puzzle_completion();
    }
}

bool check_puzzle_update() {
    Serial.println("checking puzzle update");
    bool updated = false;
    if (current_attempt.amplitude != last_amplitude) {
        last_amplitude = current_attempt.amplitude;
        updated = true;
    }
    if (current_attempt.frequency != last_frequency) {
        last_frequency = current_attempt.frequency;
        updated = true;
    }
    if (current_attempt.phase_shift != last_phase_shift) {
        last_phase_shift = current_attempt.phase_shift;
        updated = true;
    }
    return updated;
}

void check_puzzle_completion() {
    if (
        (current_attempt.amplitude == target_puzzle.amplitude) && 
        (current_attempt.frequency == target_puzzle.frequency) && 
        ((current_attempt.phase_shift - target_puzzle.phase_shift) % frequencyConverter[target_puzzle.frequency] == 0)) 
        {

        has_current_puzzle = false;
        if(DEMOMODE){
            set_waveform_puzzle(create_random_puzzle());
        }
        Serial.println("puzzle complete!");
        //report puzzle completion somehow
    }
}

void set_waveform_puzzle(waveform puzzle) {
    target_puzzle = puzzle;
    has_current_puzzle = true;
}

waveform create_random_puzzle() {
    waveform puzzle;
    puzzle.frequency = random(1, 16);
    puzzle.amplitude = random(1, 10) / 10.0f; //changed min from 0 to 1
    puzzle.phase_shift = random(0, 15);
    return puzzle;
}

waveform get_waveform_puzzle() {
    return target_puzzle;
}

void set_user_input(waveform puzzle) {
    current_attempt = puzzle;
}
