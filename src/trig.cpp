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

static waveform_puzzle current_waveform_puzzle;


uint8_t read_knob(){
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
float example_trig_function(float x){
    return sin(x);
}

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

void draw_trig(Adafruit_ILI9341 *screen, float (*trig_function)(float)){
    screen->fillScreen(screen->color565(0,0,0));
    int h = screen->height();
    int w = screen->width();
    for(int x=0; x<w; x++){
        screen->drawPixel(x, denormalize(trig_function(normalize(x))), screen->color565(74, 242, 98));
    }
}

float target_trig_function(float x) {
    //A goes from 0 to 1
    float A = get_target_aplitude();
    //k goes from 1 to 16
    uint16_t k = get_target_frequency();
    return A * sin(k * x);
}

float controlled_trig_function(float x) {
    //A goes from 0 to 1
    float A = get_user_amplitude();
    //k goes from 1 to 16
    uint16_t k = get_user_frequency();
    return A * sin(k * x);
}

float get_user_amplitude() {
    //read encoder
}

uint16_t get_user_frequency() {
    //read encoder
}

float get_target_amplitude() {
    //dunno how to do this
}

uint16_t get_target_frequency() {
    //dunno how to do this
}

void init_trig(){
    target_screen.begin();
    target_screen.setRotation(1); //width is the long way
    controlled_screen.begin();
    controlled_screen.setRotation(1);
}

void loop_trig(){
    //target_screen.fillScreen(target_screen.color565(255, 255, 255));
    if (current_puzzle) { //TODO: Is this legal?
        draw_trig(&target_screen, &example_trig_function);
        draw_trig(&controlled_screen, &controlled_trig_function);
        check_puzzle_completion();
    }
}

void check_puzzle_completion() {
    if ((get_user_amplitude() == get_target_amplitude()) && (get_user_frequency() == get_target_frequency())) {
        //report puzzle completion
        current_waveform_puzzle = None; //fix this
    }
}