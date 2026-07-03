#include <Adafruit_ILI9341.h>
#include "trig.h"
#include "MainBoardPinout.h"
#include "MainBoard.h"
#include <math.h>
#include <Adafruit_seesaw.h>
#include <Arduino.h>
#include "global_flags.h"

extern SPIClass mainBoardSpi;

static Adafruit_ILI9341 target_screen = Adafruit_ILI9341(&mainBoardSpi, MAIN_BOARD_LCD_DC, 
    MAIN_BOARD_LCD_1_CS, -1);

static Adafruit_ILI9341 controlled_screen = Adafruit_ILI9341(&mainBoardSpi, MAIN_BOARD_LCD_DC, 
    MAIN_BOARD_LCD_2_CS, -1);

static waveform target_puzzle;
static waveform current_attempt;
static boolean has_current_puzzle;
void check_puzzle_completion();
bool check_puzzle_update();
float last_amplitude;
float last_frequency;
float last_phase_shift;



uint8_t read_knob() {
    //implement this please
    //returns a value from 0 - 8
    return 1;
}
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
float normalize(uint16_t x) {
    float MIN_NUM_HORIZ_PERIODS_IN_WIDTH = 0.8;
    uint16_t w = target_screen.width();
    float x_fractional = x / (float)w;
    return x_fractional * MIN_NUM_HORIZ_PERIODS_IN_WIDTH * 2 * PI;
}

uint16_t denormalize(float y) { //y goes from -1 to 1
    float MAX_AMPLITUDE_AS_FRACTION_OF_HEIGHT = 0.7;
    uint16_t h = target_screen.height();
    float y_as_float = y * h * MAX_AMPLITUDE_AS_FRACTION_OF_HEIGHT / 2; //divide by 2 because the input has range 2
    uint16_t output =  (h/2) + (float)y_as_float;
    return output;
}

void draw_trig(Adafruit_ILI9341 *screen, waveform target, waveform attempt) {
    screen->fillScreen(screen->color565(0,0,0));
    int h = screen->height();
    int w = screen->width();
    //draw waveforms
    uint16_t last_target = 0;
    uint16_t last_attempt = 0;
    uint16_t current_target_y = denormalize(target.amplitude * sin(target.frequency * ((float) target.phase_shift + normalize(0))));
    uint16_t current_attempt_y = denormalize(attempt.amplitude * sin(attempt.frequency * ((float) attempt.phase_shift + normalize(0))));
    for(int x=1; x<w; x++){ //skip the first one because we just took care of it
        uint16_t last_target_y = current_target_y;
        uint16_t last_attempt_y = current_attempt_y;
        current_target_y = denormalize(target.amplitude * sin(target.frequency * ((float) target.phase_shift + normalize(x))));
        current_attempt_y = denormalize(attempt.amplitude * sin(attempt.frequency * ((float) attempt.phase_shift + normalize(x))));
        screen->drawLine(x-1, last_target_y, x, current_target_y, screen->color565(74, 242, 98));
        screen->drawLine(x-1, last_attempt_y, x, current_attempt_y, screen->color565(242, 74, 98));
    }
}

void init_trig() {
    pinMode(MAIN_BOARD_LCD_RESET, OUTPUT);
    digitalWrite(MAIN_BOARD_LCD_RESET, HIGH);
    target_screen.begin();
    target_screen.setRotation(1); //width is the long way
    controlled_screen.begin();
    controlled_screen.setRotation(1);
    target_screen.fillScreen(target_screen.color565(0, 255, 255));
    controlled_screen.fillScreen(controlled_screen.color565(0, 255, 255));
    Serial.println("GFX initialized");
    target_screen.fillScreen(target_screen.color565(0,255,0));
}

void loop_trig() {
    Serial.println("in trig loop");
    if (has_current_puzzle&&check_puzzle_update()) {
        draw_trig(&controlled_screen, target_puzzle, current_attempt);
        //draw_trig(&target_screen, target_puzzle);
        //draw_trig(&controlled_screen, current_user_attempt);
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
    if ((current_attempt.amplitude == target_puzzle.amplitude) && (current_attempt.frequency == target_puzzle.frequency)) {
        //doesn't check phase shift yet
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
    last_amplitude = 0; //hack to force screen update
}

waveform create_random_puzzle() {
    waveform puzzle;
    puzzle.frequency = random(1, 16);
    puzzle.amplitude = .5f;//random(0, 10) / 10.0f;
    puzzle.phase_shift = 0;//random(0, 16);
    return puzzle;
}

waveform get_waveform_puzzle() {
    return target_puzzle;
}

void set_user_input(waveform puzzle) {
    current_attempt = puzzle;
}
