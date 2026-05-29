#include <Adafruit_ILI9341.h>
#include "trig.h"
#include "MainBoardPinout.h"
#include "MainBoard.h"
#include <math.h>

extern SPIClass mainBoardSpi;

static Adafruit_ILI9341 target_screen = Adafruit_ILI9341(&mainBoardSpi, MAIN_BOARD_LCD_DC, 
    MAIN_BOARD_LCD_1_CS, MAIN_BOARD_LCD_RESET);

static Adafruit_ILI9341 controlled_screen = Adafruit_ILI9341(&mainBoardSpi, MAIN_BOARD_LCD_DC, 
    MAIN_BOARD_LCD_2_CS, MAIN_BOARD_LCD_RESET);

static waveform current_waveform_puzzle;
static waveform current_user_attempt;
static boolean has_current_puzzle;

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

uint16_t denormalize(uint16_t y) { //y goes from -1 to 1
    float MAX_AMPLITUDE_AS_FRACTION_OF_HEIGHT = 0.7;
    uint16_t h = target_screen.height();
    float y_as_float = y * h * MAX_AMPLITUDE_AS_FRACTION_OF_HEIGHT / 2; //divide by 2 because the input has range 2
    uint16_t output =  (h/2) + (float)y_as_float;
    return output;
}

void draw_trig(Adafruit_ILI9341 *screen, waveform function_to_draw) {
    screen->fillScreen(screen->color565(0,0,0));
    int h = screen->height();
    int w = screen->width();
    for(int x=0; x<w; x++){
        screen->drawPixel(x, denormalize(function_to_draw.amplitude * sin(function_to_draw.frequency * (function_to_draw.phase_shift + normalize(x)))), screen->color565(74, 242, 98));
    }
}

void init_trig() {
    target_screen.begin();
    target_screen.setRotation(1); //width is the long way
    controlled_screen.begin();
    controlled_screen.setRotation(1);
}

void loop_trig() {
    //target_screen.fillScreen(target_screen.color565(255, 255, 255));
    if (has_current_puzzle) {
        draw_trig(&target_screen, current_waveform_puzzle);
        draw_trig(&controlled_screen, current_user_attempt);
        check_puzzle_completion();
    }
}

void check_puzzle_completion() {
    if ((current_user_attempt.amplitude == current_waveform_puzzle.amplitude) && (current_user_attempt.frequency == current_waveform_puzzle.frequency)) {
        //doesn't check phase shift yet
        has_current_puzzle = false;
        //report puzzle completion somehow
    }
}

void set_waveform_puzzle(waveform puzzle) {
    current_waveform_puzzle = puzzle;
}

waveform get_waveform_puzzle() {
    return current_waveform_puzzle;
}

void set_user_input(waveform puzzle) {
    current_user_attempt = puzzle;
}

waveform get_user_input() {
    return current_user_attempt;
}