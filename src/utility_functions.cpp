#include <Arduino.h>
#include <Timezone.h>

#include "functions.h"

#define makeStringFromUINT(...) __VA_ARGS__ < 10 ? "0" + String(__VA_ARGS__) : String(__VA_ARGS__)

// Prepare string with current state of the lamp.
// Status format:
// <state (power)>; <effect (mode)>; <brightness>; <speed>; <scale>
String lamp_state_2_string(the_lamp_state_t *lamp_state)
{
    String result = "";

    if (lamp_state->state)
    {
        result += "1;";
    }
    else
    {
        result += "0;";
    }

    result += lamp_state->effect;
    result += ";";
    result += lamp_state->brightness_raw;
    result += ";";
    result += lamp_state->speed_raw;
    result += ";";
    result += lamp_state->scale_raw;

    DPRINTF("lamp_state_2_string: {");
    DPRINT(result);
    DPRINTLNF("}");

    return result;
}

void printDecNum(uint32_t num)
{
    DPRINT((num < 10) ? "0" : "");
    DPRINT(num);
}

uint32_t getCurrentDateTime(the_lamp_state_t &lamp_state)
{
    uint32_t curr_time = lamp_state.date_time->local_time + (millis()-lamp_state.date_time->local_millis)/10000;
    return curr_time;
}

uint32_t getCurrentDateTime(the_lamp_state_t &lamp_state, uint32_t localDateTime)
{
    uint32_t curr_time = lamp_state.date_time->local_time + (localDateTime-lamp_state.date_time->local_millis)/10000;
    return curr_time;
}

String makeStringDateTime(uint32_t localDateTime)
{
    // 2012-04-23T18:25:43.511Z
    String DateTimeStr = "";

    DateTimeStr += makeStringDate(localDateTime);
    DateTimeStr += F("T");
    DateTimeStr += makeStringTime(localDateTime);
    DateTimeStr += F("+03:00");
    // DateTimeStr += F("Z");

    return DateTimeStr;
}

String makeStringDate(uint32_t localDateTime)
{
    String DateStr = "";
    uint8_t localMouth = month(localDateTime);
    uint8_t localDay = day(localDateTime);

    DateStr += year(localDateTime);
    DateStr += F("-");
    DateStr += makeStringFromUINT(localMouth);
    DateStr += F("-");
    DateStr += makeStringFromUINT(localDay);

    return DateStr;
}

String makeStringTime(uint32_t localDateTime)
{
    String TimeStr = "";
    uint8_t localHour = hour(localDateTime);
    uint8_t localMin = minute(localDateTime);
    uint8_t localSec = second(localDateTime);

    TimeStr += makeStringFromUINT(localHour);
    TimeStr += F(":");
    TimeStr += makeStringFromUINT(localMin);
    TimeStr += F(":");
    TimeStr += makeStringFromUINT(localSec);

    return TimeStr;
}

//void printCurrentDateTime(local_date_time_t *date_time) {
void printDateTimeStruct(local_date_time_t *date_time)
{
    DPRINTLNF("printCurrentDateTime()");
    DPRINTF("local_millis = ");
    DPRINTLN(date_time->local_millis);
    DPRINTF("local_time = ");
    DPRINTLN(date_time->local_time);
    // DPRINTF("Current day: ");
    // DPRINTLN(date_time->day);
    DPRINTF("Current time: ");
    DPRINT(hour(date_time->local_time));
    DPRINTF(":");
    DPRINT(minute(date_time->local_time));
    DPRINTF(":");
    DPRINTLN(second(date_time->local_time));
}

void printTime(time_t currentLocalTime)
{
    // DPRINTLNF("printCurrentLocalTime()");
    DPRINTF("currentLocalTime = ");
    DPRINTLN(currentLocalTime);
    DPRINTF("Current time: ");
    printDecNum(hour(currentLocalTime));
    DPRINTF(":");
    printDecNum(minute(currentLocalTime));
    DPRINTF(":");
    printDecNum(second(currentLocalTime));
    DPRINTLNF("");
}

void printString(const char *charArray)
{
    #ifdef DEBUG_PRINT
        DPRINTF("Name = {");
        String tmpStr = charArray;
        DPRINT(tmpStr.c_str());
        DPRINTF("}");

        DPRINTF(" (length = ");
        DPRINT(tmpStr.length());
        DPRINTLNF(")");
    #endif
}
//////////////////////////////////////////////////////
// Internal functions, use they through 'API' only! //
//////////////////////////////////////////////////////

// The Time library uses day of the week in the European numbering (days numbered from Sunday).
// So we have to convert it into the standard russian form (also compatible with ISO-8601).
uint8_t convert_to_ISO8601(uint8_t EuropeanDay)
{
    uint8_t thisDay = EuropeanDay;
    if (thisDay == 1)
    {
        thisDay = 8;
    }
    thisDay -= 2;
    return thisDay;
}

uint8_t convert_from_ISO8601(uint8_t ISO8601Day)
{
    uint8_t thisDay = ISO8601Day;
    thisDay += 2;
    if (thisDay == 8)
    {
        thisDay = 1;
    }
    return thisDay;
}

uint32_t str2int(char *str, uint8_t len)
{
    uint32_t res = 0;

    for (byte i = 0; i < len; i++)
    {
        if (('0' <= str[i]) && (str[i] <= '9'))
            res = 10 * res + (str[i] - '0');
        else
            return res;
    }
    return res;
}

// Change the parameter's value
// Input parameters:
//    value - current value: unsigned integer inside [0:255]
//    delta - signed delta to change (negative value for decreasing, positive for increasing)
//    limit - upper limit (255 if not set)
//    saturation - boundary cases behavior:
//      true:  value will remain in set boundaries
//      false: value will cross around boundaries from minimum (maximum) to maximum (minimum)
// Output value:
//    returned value will be inside [0:limit]
uint16_t ChangeParameterValue(uint16_t value, int8_t delta, uint16_t limit /*= 255*/, boolean saturation /*= true*/)
{
    int16_t new_value = (int16_t)(value + delta);

    // Low limit is always 0
    if (new_value < 0)
    {
        if (saturation)
            new_value = 0;
        else
            new_value = (int16_t)limit;
    }

    // Upper limit may be less than 255, see input parameter
    if (new_value > (int16_t)limit)
    {
        if (saturation)
            new_value = limit;
        else
            new_value = 0;
    }

    return new_value;
}

// Shift matrix 'down' (-1 by Y)
void shiftMatrixDown(CRGB *leds)
{
    // for (uint8_t x = 0; x <= WIDTH - 1; x++)
    //   for (uint8_t y = 0; y <= HEIGHT - 2; y++)
    //     drawPixelXY(x, y, getPixelColorXY(x, y + 1));

    for (uint8_t x = 0; x <= WIDTH - 1; x++)
        for (uint8_t y = 0; y <= HEIGHT - 2; y++)
            leds[getPixelNumber(x, y)] = leds[getPixelNumber(x, y + 1)];
}

// Shift matrix 'up' (+1 by Y)
void shiftMatrixUp(CRGB *leds)
{
    // for (uint8_t x = 0; x <= WIDTH - 1; x++)
    //   for (uint8_t y = HEIGHT - 1; y >= 1; y--)
    //     drawPixelXY(x, y, getPixelColorXY(x, y + 1));

    for (uint8_t x = 0; x <= WIDTH - 1; x++)
        for (uint8_t y = HEIGHT - 1; y >= 1; y--)
            leds[getPixelNumber(x, y)] = leds[getPixelNumber(x, y - 1)];
}

// Shift matrix 'right' (+1 by X)
void shiftMatrixRight(CRGB *leds)
{
    // for (uint8_t x = WIDTH - 1; x >= 1; x--)
    //   for (uint8_t y = 0; y <= HEIGHT - 1; y++)
    //     drawPixelXY(x, y, getPixelColorXY(x-1, y));

    for (uint8_t x = WIDTH - 1; x >= 1; x--)
        for (uint8_t y = 0; y <= HEIGHT - 1; y++)
            leds[getPixelNumber(x, y)] = leds[getPixelNumber(x - 1, y)];
}

// Shift matrix 'left' (-1 by X)
void shiftMatrixLeft(CRGB *leds)
{
    // for (uint8_t x = 0; x <= WIDTH - 2; x++)
    //   for (uint8_t y = 0; y <= HEIGHT - 1; y++)
    //     drawPixelXY(x, y, getPixelColorXY(x-1, y));

    for (uint8_t x = 0; x <= WIDTH - 2; x++)
        for (uint8_t y = 0; y <= HEIGHT - 1; y++)
            leds[getPixelNumber(x, y)] = leds[getPixelNumber(x + 1, y)];
}

// Shift matrix diagonally 'down-right' (+1 by X, -1 by Y)
void shiftMatrixDownRight(CRGB *leds)
{
    for (uint8_t x = WIDTH - 1; x >= 1; x--)
        for (uint8_t y = 0; y <= HEIGHT - 2; y++)
            drawPixelXY(leds, x, y, getPixelColorXY(leds, x - 1, y + 1));
}

// Attenuation all pixels in the matrix
void fadeMatrix(CRGB *leds, uint8_t step)
{
    for (uint8_t i = 0; i < WIDTH; i++)
        for (uint8_t j = 0; j < HEIGHT; j++)
            fadePixel(leds, i, j, step);
}

// Attenuation the pixel: check the current pixel's brightness and reduce it
void fadePixel(CRGB *leds, uint8_t x, uint8_t y, uint8_t step)
{
    uint16_t pixelNum = getPixelNumber(x, y);
    if (getPixelColor(leds, pixelNum) == 0)
        return;

    if (leds[pixelNum].r >= 16 ||
        leds[pixelNum].g >= 16 ||
        leds[pixelNum].b >= 16)
        leds[pixelNum].fadeToBlackBy(step);
    else
        leds[pixelNum] = 0;
}

// Attenuation all pixels in the matrix
void fadeMatrixManually(CRGB *leds, uint8_t step)
{
    for (uint8_t i = 0; i < WIDTH; i++)
        for (uint8_t j = 0; j < HEIGHT; j++)
            fadePixelManually(leds, i, j, step);
}

// Attenuation the pixel: check the current pixel's brightness and reduce it
void fadePixelManually(CRGB *leds, uint8_t x, uint8_t y, uint8_t step)
{
    register uint16_t pixelNum = getPixelNumber(x, y);
    // if (getPixelColor(pixelNum) == 0)
    //   return;

    if (leds[pixelNum].r >= step)
        leds[pixelNum].r -= step;
    else
        leds[pixelNum].r = 0;

    if (leds[pixelNum].g >= step)
        leds[pixelNum].g -= step;
    else
        leds[pixelNum].g = 0;

    if (leds[pixelNum].b >= step)
        leds[pixelNum].b -= step;
    else
        leds[pixelNum].b = 0;
}

// Fill the matrix by color
void fillAllbyColor(CRGB *leds, CRGB color)
{
    for (int i = 0; i < NUM_LEDS; i++)
        leds[i] = color;
}

// Randomly filling the matrix with color points
void fillRandomly(CRGB *leds, uint16_t amount)
{
    uint8_t count = 0;
    uint8_t try_count = 0;

    while ((count < amount) && (try_count < 255))
    {
        register uint8_t x = random(0, WIDTH);
        register uint8_t y = random(0, HEIGHT);
        register uint16_t index = getPixelNumber(x, y);

        if (getPixelColor(leds, index) == 0)
        {
            // leds[index] = CHSV(random(0, 256), 255, 255);
            leds[index] = CHSV((random(1, 17) * 16 - 1), 255, 255);
            count++;
        }
        try_count++;
    }
}

// Randomly filling the full matrix with color points
void fillRandomlyAll(CRGB *leds)
{
    for (int i = 0; i < NUM_LEDS; i++)
        leds[i] = CHSV((random(1, 17) * 16 - 1), 255, 255);
}

// Set color of the pixel 'in strip' by index
void drawPixel(CRGB *leds, uint16_t index, CRGB color)
{
#ifdef USE_PARAMS_CHECK
    if (index <= NUM_LEDS - 1)
        leds[index] = color;
#else
    leds[index] = color;
#endif
}

// Set color of the pixel 'in matrix' by X,Y coordinates
void drawPixelXY(CRGB *leds, uint8_t x, uint8_t y, CRGB color)
{
#ifdef USE_PARAMS_CHECK
    if (x <= WIDTH - 1 || y <= HEIGHT - 1)
        leds[getPixelNumber(x, y)] = color;
#else
    leds[getPixelNumber(x, y)] = color;
#endif
}

// Get color of the pixel 'in strip' by index
uint32_t getPixelColor(CRGB *leds, uint16_t index)
{
#ifdef USE_PARAMS_CHECK
    if (index <= NUM_LEDS - 1)
        return (uint32_t)leds[index];
    return 0;
#else
    // return (uint32_t)leds[index];
    return (((uint32_t)leds[index].r << 16) | ((uint16_t)leds[index].g << 8) | (uint16_t)leds[index].b);
#endif
}

// Get color of the pixel 'in strip' by X,Y coordinates
uint32_t getPixelColorXY(CRGB *leds, uint8_t x, uint8_t y)
{
    register uint16_t index = getPixelNumber(x, y);
#ifdef USE_PARAMS_CHECK
    if (x <= WIDTH - 1 || y <= HEIGHT - 1)
        return (uint32_t)leds[getPixelNumber(x, y)];
    return 0;
#else
    // return (uint32_t)leds[getPixelNumber(x, y)];
    return (((uint32_t)leds[index].r << 16) | ((uint16_t)leds[index].g << 8) | (uint16_t)leds[index].b);
#endif
}

// Get the pixel number in the tape by coordinates
uint16_t getPixelNumber(uint8_t x, uint8_t y)
{
    if ((THIS_Y % 2 == 0) || MATRIX_TYPE) // even line
        return (THIS_WIDTH * THIS_Y + THIS_X);
    else // odd line
        return (THIS_WIDTH * THIS_Y + THIS_WIDTH - THIS_X - 1);
}
