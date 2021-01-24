#ifndef __INC_WORK_FUNCTIONS_H
#define __INC_WORK_FUNCTIONS_H 

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <NTPClient.h>

#include <FastLED.h>

#include "Michael_MinimalTimer.h"

#include "project_config.h"
#include "effects.h"
#include "matrix.h"

typedef struct {
    boolean  synchronized = false;
    uint8_t  day          = 0;
    uint8_t  hour         = 0;
    uint8_t  minute       = 0;
    uint8_t  second       = 0;
    time_t   local_time   = 0;
    uint32_t local_millis = 0;
} local_date_time_t;

typedef struct {
    boolean  state          = false; // ON/OFF
    boolean  debug          = false; // Debug mode: manual stepping instead of timer if TRUE
    boolean  step           = false; // Run next step if debug=true
    uint8_t  effect         = 0;     // Current effect number
    uint8_t  brightness     = 0;     // Matrix brightness
    uint8_t  brightness_raw = 0;     // Raw brightness value (from GUI)
    uint32_t speed          = 0;     // Current effect speed
    uint8_t  speed_raw      = 0;     // Raw speed value (from GUI)
    uint16_t scale          = 0;     // Current effect scale
    uint8_t  scale_raw      = 0;     // Raw scale value (from GUI)


    String   IP             = "";    // IP
    boolean  loadingFlag    = false; // First-run/redrawing efffect flag

    local_date_time_t *date_time   = NULL;
    M_MinimalTimer    *effectTimer = NULL; // Effect timer for rendering
    CRGB              *leds        = NULL;
} the_lamp_state_t;

// Lamp state machine
typedef enum {
    FSM_IDLE=0,
    FSM_CHANGE_MODE=1
} controlFSM_t;

typedef struct {
    char get_prefix[4];
    char split_1;
    char type[3];
    char split_2;
    char param[4];
} get_command_t;

typedef struct {
    char get_prefix[5];
    char split_1;
    char type[3];
    char split_2;
    char param[4];
} post_command_t;

// Alarm possible state coding
typedef enum {
    alarm_off=0,
    alarm_once=1,
    alarm_weekly=2
} alarm_state_t;

typedef struct {
    alarm_state_t state = alarm_off;
    uint8_t  hour       = 0;
    uint8_t  min        = 0;
} alarm_t;

// Functions
    // work API
void changePower(the_lamp_state_t *lamp_state);
void SelectEffect(the_lamp_state_t *lamp_state);
void ResetEffectSettings(the_lamp_state_t *lamp_state);

void updateMode(the_lamp_state_t *lamp_state);
void setMode(the_lamp_state_t *lamp_state, uint8_t Value);
void changeMode(the_lamp_state_t *lamp_state, int8_t ChangeValue);
void nextMode(the_lamp_state_t *lamp_state);
void prevMode(the_lamp_state_t *lamp_state);
void setBrightness(the_lamp_state_t *lamp_state, uint8_t Value);
void changeBrightness(the_lamp_state_t *lamp_state, int8_t ChangeValue);
void setEffectsSpeed(the_lamp_state_t *lamp_state, uint32_t Value);
void changeEffectsSpeed(the_lamp_state_t *lamp_state, int8_t ChangeValue);
void convertRAW(the_lamp_state_t *lamp_state);

    // utility functions declaration
void     printDecNum(uint32_t num);
void     printDateTimeStruct(local_date_time_t *date_time);
void     printTime(time_t currentLocalTime);
uint8_t  convert_to_ISO8601(uint8_t EuropeanDay);
uint8_t  convert_from_ISO8601(uint8_t ISO8601Day);
uint32_t str2int(char *str, uint8_t len);
String   lamp_state_2_string(the_lamp_state_t *lamp_state);
uint16_t ChangeParameterValue(uint16_t value, int8_t delta, uint16_t limit=255, boolean saturation = true); // Change the parameter's value by signed delta
        // Matrix filling
void     fillAllbyColor(CRGB *leds, CRGB color);                            // Fill the matrix by color
void     fillRandomly(CRGB *leds, uint16_t amount);                         // Fill the matrix with random points by random color
void     fillRandomlyAll(CRGB *leds);                                       // Fill the matrix by random colors
        // Matrix-to-strip coordinates conversion
uint16_t getPixelNumber(uint8_t x, uint8_t y);                              // Get the pixel number in the tape by coordinates
        // Operation with pixel's color
void     drawPixel(CRGB *leds, uint16_t index, CRGB color);                 // Set color of the pixel 'in strip' by index
void     drawPixelXY(CRGB *leds, uint8_t x, uint8_t y, CRGB color);         // Set color of the pixel 'in matrix' by X,Y coordinates
uint32_t getPixelColor(CRGB *leds, uint16_t index);                         // Get color of the pixel 'in strip' by index
uint32_t getPixelColorXY(CRGB *leds, uint8_t x, uint8_t y);                 // Get color of the pixel 'in strip' by X,Y coordinates
        // Colors' attenuation
void     fadePixel(CRGB *leds, uint8_t x, uint8_t y, uint8_t step);         // Attenuation of the pixel: check the current pixel's brightness and reduce it; calls fadeToBlackBy function from fastLED library
void     fadeMatrix(CRGB *leds, uint8_t step);                              // Attenuation of all pixels in matrix
void     fadePixelManually(CRGB *leds, uint8_t x, uint8_t y, uint8_t step); // Attenuation of the pixel: check the current pixel's brightness and reduce it
void     fadeMatrixManually(CRGB *leds, uint8_t step);
        // Shift/rotate matrix
void     shiftMatrixDown(CRGB *leds);                                       // Shift matrix down  on 1 by Y
void     shiftMatrixUp(CRGB *leds);                                         // Shift matrix up    on 1 by Y
void     shiftMatrixRight(CRGB *leds);                                      // Shift matrix right on 1 by X
void     shiftMatrixLeft(CRGB *leds);                                       // Shift matrix left  on 1 by X
void     shiftMatrixDownRight(CRGB *leds);                                  // Shift matrix down-right on 1 by X an dY

    // Time
boolean NTP_Synchronization(NTPClient *timeClient, local_date_time_t *date_time);
boolean CheckInternetAccess();

    // Effects
        // Three auxiliary functions for fire 
void generateLine();
void shiftUp();
void drawFrame(CRGB *leds, int pcnt);
        // Main functions
void snowRoutine(the_lamp_state_t *lamp_state);
void ballsRoutine(the_lamp_state_t *lamp_state);
void fireRoutine(the_lamp_state_t *lamp_state);
void sparklesRoutine(the_lamp_state_t *lamp_state);
void matrixRoutine(the_lamp_state_t *lamp_state);
void starfallRoutine(the_lamp_state_t *lamp_state);

    // fullscreen effects
        // Three auxiliary functions for fullscreen effects
void SetupRandomPalette();
void SetupBlackAndWhiteStripedPalette();
void SetupPurpleAndGreenPalette();
void fillNoiseLED(CRGB *leds);
void fillnoise8();
void mapNoiseToLEDsUsingPalette(CRGB *leds);
        // Main functions
void lavaNoise(the_lamp_state_t *lamp_state);
void cloudNoise(the_lamp_state_t *lamp_state);
void zebraNoise(the_lamp_state_t *lamp_state);
void forestNoise(the_lamp_state_t *lamp_state);
void oceanNoise(the_lamp_state_t *lamp_state);
void plasmaNoise(the_lamp_state_t *lamp_state);
void plumNoise(the_lamp_state_t *lamp_state);
void randomNoise(the_lamp_state_t *lamp_state);
void rainbowNoise(the_lamp_state_t *lamp_state);
void rainbowStripeNoise(the_lamp_state_t *lamp_state);

    // Games
        // Snake
void newSnake(CRGB *leds);
void newApple(CRGB *leds);
void newGameSnake(CRGB *leds);
void calculate_new_coordinate(int8_t* x, int8_t* y, snake_motion_t vector);
boolean check_fail(CRGB *leds, int8_t x, int8_t y);
uint16_t shift_pointer(uint16_t pointer);
snake_motion_t change_direction(snake_motion_t current_direction);
snake_motion_t snake_brain(CRGB *leds);
void snakeRoutine(the_lamp_state_t *lamp_state);

#endif // __INC_WORK_FUNCTIONS_H