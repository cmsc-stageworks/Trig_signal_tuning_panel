#include <Adafruit_ILI9341.h>
#include <Adafruit_ST7796S.h>
#include "trig.h"
#include "MainBoardPinout.h"
#include "MainBoard.h"
#include <math.h>
#include <Adafruit_seesaw.h>
#include <Arduino.h>
#include "global_flags.h"
#include <esp_random.h>

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
#define PHASE_OFFSET_PERIOD 600
#define TRIG_SCREEN_WIDTH  480
//should be the wavelength of a "period one" wave; all others are integer fractions of this

static int8_t random_noise_buffer[TRIG_SCREEN_WIDTH]; //120

float normalize(uint16_t x) {
    float MIN_NUM_HORIZ_PERIODS_IN_WIDTH = 0.8;
    uint16_t w = TRIG_SCREEN_WIDTH;
    float x_fractional = (x - (w/2)) / (float)w;
    return x_fractional * MIN_NUM_HORIZ_PERIODS_IN_WIDTH * 2 * PI;
}

#define GET_RANDOM_NOISE(index, noise_level) ((noise_level) > 0 ? ((int32_t) (random_noise_buffer[(index)])) * ((int32_t) (noise_level)) / (127.0f) : 0.0f);

float denormalize(float y) { //y goes from -1 to 1
    float MAX_AMPLITUDE_AS_FRACTION_OF_HEIGHT = 0.7;
    uint16_t h = screen.height(); //320
    float y_as_float = y * h * MAX_AMPLITUDE_AS_FRACTION_OF_HEIGHT / 2; //divide by 2 because the input has range 2
    return y_as_float;
}

static const uint8_t frequencyConverter[16] = {0,1,2,3,4,5,6,7,8,9,10,12,14,16,18,20};

float calculate_y(waveform wave, uint16_t x_position, uint16_t phase_offset) {
    return denormalize((wave.amplitude / 10.0f) * sin((2 * PI * ((float) wave.phase_shift) / 16.0f) + frequencyConverter[wave.frequency] * normalize(x_position + phase_offset)));
}



void draw_trig(Adafruit_ST7796S *screen, waveform target, waveform attempt, waveform old_attempt) {
    esp_fill_random(random_noise_buffer, TRIG_SCREEN_WIDTH);
    static int8_t old_target_line[TRIG_SCREEN_WIDTH];
    static uint16_t phase_offset = 0;
    #define phase_offset_delta SCANSPEED
    int h = screen->height();
    int w = screen->width();
    //draw waveforms
    uint16_t old_last_attempt_y = 0;
    uint16_t last_target_y = 0;
    uint16_t last_attempt_y = 0;
    uint16_t old_current_attempt_y = calculate_y(old_attempt, 0, phase_offset);
    uint16_t current_attempt_y = calculate_y(attempt, 0, phase_offset + phase_offset_delta); 
    uint16_t current_target_y = calculate_y(target, 0, phase_offset + phase_offset_delta) + GET_RANDOM_NOISE(0, target.noise_level);
    for(int x=1; x<w; x++){ //skip the first one because we just took care of it
        old_last_attempt_y = old_current_attempt_y;
        last_attempt_y = current_attempt_y;
        last_target_y = current_target_y;
        old_current_attempt_y = calculate_y(old_attempt, x, phase_offset);
        current_attempt_y = calculate_y(attempt, x, phase_offset + phase_offset_delta);
        current_target_y = calculate_y(target, x, phase_offset + phase_offset_delta) + GET_RANDOM_NOISE(x, target.noise_level);
        screen->drawLine(x-1, old_target_line[x-1] + (h / 2), x, old_target_line[x] + (h / 2), 0); //draw over the last line
        if ((((x + phase_offset)/(old_attempt.frequency >= 8 ? 1 : 3)) % 2) == 0) {
            screen->drawLine(x-1, old_last_attempt_y + (h / 2), x, old_current_attempt_y + (h / 2), 0);
        }
        screen->drawLine(x-1, last_target_y + (h / 2), x, current_target_y + (h / 2), screen->color565(74, 242, 98));
        if ((((x + phase_offset + phase_offset_delta)/(attempt.frequency >= 8 ? 1 : 3)) % 2) == 0) {
            screen->drawLine(x-1, last_attempt_y + (h / 2), x, current_attempt_y + (h / 2), screen->color565(242, 74, 98));
        }
        old_target_line[x-1] = last_target_y;
    }
    old_target_line[w - 1] = current_target_y;
    phase_offset = (phase_offset + phase_offset_delta) % PHASE_OFFSET_PERIOD;
}

void init_trig() {
    screen.init(320, 480, 0, 0, ST7796S_BGR);
    screen.setRotation(1); //width is the long way
    screen.invertDisplay(true);
    screen.fillScreen(screen.color565(0, 255, 255));
    Serial.println("GFX initialized");
    screen.fillScreen(screen.color565(0,0,0));
}

void loop_trig() {
    if (has_current_puzzle) { 
        //check_puzzle_update is now deprecated because the puzzle now updates every frame, due to scrolling
        static waveform old_target = target_puzzle, last_attempt = current_attempt;
        draw_trig(&screen, target_puzzle, current_attempt, last_attempt);
        old_target = target_puzzle;
        last_attempt = current_attempt;
        check_puzzle_completion();
    }
}

void check_puzzle_completion() {
    if (
        (current_attempt.amplitude == target_puzzle.amplitude) && 
        (current_attempt.frequency == target_puzzle.frequency) && 
        (current_attempt.phase_shift == target_puzzle.phase_shift))
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
    puzzle.amplitude = random(1, 10); //changed min from 0 to 1
    puzzle.phase_shift = random(0, 15);
    puzzle.noise_level = random(0, 12);
    return puzzle;
}

waveform get_waveform_puzzle() {
    return target_puzzle;
}

void set_user_input(waveform puzzle) {
    current_attempt = puzzle;
}
