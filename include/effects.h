#ifndef __INC_EFFECTS_H
#define __INC_EFFECTS_H

#include <Arduino.h>

#include <FastLED.h>

#define EFF_CODE_SNOW            0      // Snowfall
#define EFF_CODE_BALLS           1      // Balls-lichters
#define EFF_CODE_FIRE            2      // Fire
#define EFF_CODE_SPARKLES        3      // Sparkles
#define EFF_CODE_MATRIX          4      // The Matrix
#define EFF_CODE_STARFALL        5      // Starfall
#define EFF_CODE_SNAKE           6      // Snake game
#define EFF_CODE_LAVA            7      // Lava 3D
#define EFF_CODE_CLOUD           8      // Cloud 3D
#define EFF_CODE_ZEBRA           9      // Zebra 3D
#define EFF_CODE_FOREST         10      // Forest 3D
#define EFF_CODE_OCEAN          11      // Ocean 3D
#define EFF_CODE_PLASMA         12      // Plasma 3D
#define EFF_CODE_PLUM           13      // Plum 3D
#define EFF_CODE_RANDOM         14      // Random colors 3D
#define EFF_CODE_RAINBOW        15      // Rainbow 3D
#define EFF_CODE_RAINBOW_STRIPE 16      // Rainbow stripe 3D
#define EFF_DAWN                31      // Dawn

// Effects' configuration
    // "Snow"
#define SNOW_DENSE              10       // density
#define SNOW_COLOR        0xE0FFFF       // color: LightCyan
#define SNOW_COLOR_STEP   0x101010       // unit gradient of color
// #define SNOW_EFFECT_SPEED EFFECT_SPEED // speed by default

    // "Balls"
#define BALLS_MAX_AMOUNT    32  // Maximum amount of balls
#define BALLS_AMOUNT         5  // Amount of balls
#define BALLS_USE_TRACK      1  // ON/OFF tracks (traces) of balls
#define BALLS_TRACK_LENGTH 128  // Length of balls track (in range [1:255])
#define BALLS_MIN_SPEED      4  // 
#define BALLS_MAX_SPEED     24  // 
//#define BALLS_DRAW_WALLS   0  // режим с рисованием препятствий для шаров (не работает на ESP и STM32)
    // "Fire"
#define FIRE_SPARKLES 1        // ON/OFF emitting coals
#define FIRE_HUE_ADD  0        // Adding a color to fire  (in range [0:230] - changes the whole color of the fire)
    // "Sparkles"
#define FLARE_DENSITY           5  // density
#define FLARE_COLOR_STEP     0x18  // (!) unit gradient of color (0x101010)
#define FLARE_COLOR_STEP_LIB   48
//random color fading flash
    // "The Matrix"
#define THE_MATRIX_COLOR      0x00FF00 // color: Green
#define THE_MATRIX_DENSITY          10 // density: min 1, max 10
#define THE_MATRIX_LENGTH           12 // Length of 'line' track (in range [4:16])
// #define THE_MATRIX_COLOR_STEP 256/THE_MATRIX_LENGTH  // unit gradient of color
    // "Meteor Shower"
#define METEOR_MAX_OBJECTS   16  // absolute maximum meteors in matrix
#define METEOR_DENSITY        8  // density of meteors (maximum meteors in matrix at the same time): min 1, max METEOR_MAX_OBJECTS
#define METEOR_SATURATION   256  // color saturation (in range [32:255])
#define METEOR_TAIL_LENGTH   12  // unit gradient of color (0x181818)

#define METEOR_MIN_SPEED      4
#define METEOR_MAX_SPEED     24
#define METEOR_DELTA_SPEED    6
#define METEOR_EDGE_NUMBER   HEIGHT * 3 / 4 + WIDTH * 3 / 4 - 1

typedef struct {
    int32_t  x;
    int32_t  y;
    int32_t  speed_vector_x;
    int32_t  speed_vector_y;
    uint8_t  track_head;
    uint8_t  track_tail;
    uint8_t  track_fade;
    CRGB     color;
    boolean  exist;
    boolean  visible;
} meteor_object_t;

typedef struct {
    uint8_t  x;
    uint8_t  y;
    boolean   exist;
} track_object_t;

// Fullscreen effects' configuration
    // Main parameters: 'scale' and 'speed';
    // 'scale' - the picture is larger if smaller
    // 'spped' - "inverted" speed, 1 for a very slow moving effect
#define LAVA_SPEED         8
#define LAVA_SCALE        50

#define CLOUD_SPEED        4
#define CLOUD_SCALE       30

#define ZEBRA_SPEED       20
#define ZEBRA_SCALE       30

#define FOREST_SPEED       8
#define FOREST_SCALE     120

#define OCEAN_SPEED       20
#define OCEAN_SCALE       90

#define PLASMA_SPEED      20
#define PLASMA_SCALE      30

#define PLUM_SPEED        10
#define PLUM_SCALE        50

#define RANDOM_SPEED      20
#define RANDOM_SCALE      20

#define RAINBOW_SPEED     20
#define RAINBOW_SCALE     30

#define RAINBOW_STR_SPEED 20
#define RAINBOW_STR_SCALE 30

// Games configuration
#define SNAKE_START_LENGTH   4              // Initial length of snake
#define SNAKE_MAX_LENGTH     80             // Mmaximum length of snake
#define SNAKE_FIELD_COLOR    CRGB::Black    // Color of field
#define SNAKE_BODY_COLOR     CRGB::Green    // Color of snake
#define SNAKE_APPLE_COLOR    CRGB::Orange   // Color of apple
    // Snake's vector
typedef enum {
    s_v_Right=0,    // x++
    s_v_Left=1,     // x--
    s_v_Up=2,       // y++
    s_v_Down=3      // y--
} snake_motion_t;


// свой список режимов

// ************************ НАСТРОЙКИ ************************
// #define SMOOTH_CHANGE 1     // плавная смена режимов через чёрный
// #define SHOW_FULL_TEXT 1    // не переключать режим, пока текст не покажется весь
// #define SHOW_TEXT_ONCE 1    // показывать бегущий текст только 1 раз

// подключаем внешние файлы с картинками
//#include "bitmap2.h"


/*
-   Режимы:
    clockRoutine();       // часы на чёрном фоне

-  Игры:
    snakeRoutine();     // змейка
    tetrisRoutine();    // тетрис
    mazeRoutine();      // лабиринт
    runnerRoutine();    // бегалка прыгалка
    flappyRoutine();    // flappy bird
    arkanoidRoutine();  // арканоид

-  Бегущая строка:
    fillString("Ваш текст", цвет);    // цвет вида 0x00ff25 или CRGB::Red и проч. цвета
    fillString("Ваш текст", 1);       // радужный перелив текста
    fillString("Ваш текст", 2);       // каждая буква случайным цветом!

-  Рисунки и анимации:
    loadImage(<название массива>);    // основная функция вывода картинки
    imageRoutine1();                  // пример использования
    animation1();                     // пример анимации

*/

#endif // __INC_EFFECTS_H