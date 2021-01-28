#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include "functions.h"

void ResetEffectSettings(the_lamp_state_t *lamp_state) {
    switch (lamp_state->effect) {
        case EFF_CODE_SNOW:
            lamp_state->scale = SNOW_DENSE;
            break;
        case EFF_CODE_BALLS:
            lamp_state->scale = BALLS_AMOUNT;
            break;
        case EFF_CODE_FIRE: 
            break;
        case EFF_CODE_SPARKLES:
            lamp_state->scale = FLARE_DENSITY;
            break;
        case EFF_CODE_MATRIX:
            lamp_state->scale = THE_MATRIX_DENSITY;
            break;
        case EFF_CODE_STARFALL:
            lamp_state->scale = METEOR_DENSITY;
            break;
        case EFF_CODE_SNAKE:
            break;
        case EFF_CODE_LAVA:
            lamp_state->scale = LAVA_SCALE;
            break;
        case EFF_CODE_CLOUD:
            lamp_state->scale = CLOUD_SCALE;
            break;
        case EFF_CODE_ZEBRA:
            lamp_state->scale = ZEBRA_SCALE;
            break;
        case EFF_CODE_FOREST:
            lamp_state->scale = FOREST_SCALE;
            break;
        case EFF_CODE_OCEAN:
            lamp_state->scale = OCEAN_SCALE;
            break;
        case EFF_CODE_PLASMA:
            lamp_state->scale = PLASMA_SCALE;
            break;
        case EFF_CODE_PLUM:
            lamp_state->scale = PLUM_SCALE;
            break;
        case EFF_CODE_RANDOM:
            lamp_state->scale = RANDOM_SCALE;
            break;
        case EFF_CODE_RAINBOW:
            lamp_state->scale = RAINBOW_SCALE;
            break;
        case EFF_CODE_RAINBOW_STRIPE:
            lamp_state->scale = RAINBOW_STR_SCALE;
            break;
    }
    setBrightness(lamp_state, BRIGHTNESS);
    setEffectsSpeed(lamp_state, EFFECT_SPEED);
    updateMode(lamp_state);
    convertRAW(lamp_state);
}

void changePower(the_lamp_state_t *lamp_state) {
    DPRINTF("changePower: lamp_state->state=");
    DPRINTLN(lamp_state->state);
    
    if (lamp_state->state) {
        SelectEffect(lamp_state);
        for (int i = 0; i < BRIGHTNESS; i += 8) {
            FastLED.setBrightness(i);
            delay(10);
            FastLED.show();
        }
        FastLED.setBrightness(BRIGHTNESS);
        delay(20);
        FastLED.show();
    } 
    else {
        SelectEffect(lamp_state);
        for (int i = BRIGHTNESS; i > 8; i -= 8) {
            FastLED.setBrightness(i);
            delay(10);
            FastLED.show();
        }
        FastLED.clear();
        delay(20);
        FastLED.show();
    }
}

void SelectEffect(the_lamp_state_t *lamp_state) {
    switch (lamp_state->effect) {
        case EFF_CODE_SNOW: 
            snowRoutine(lamp_state);
            break;
        case EFF_CODE_BALLS:
            ballsRoutine(lamp_state);
            break;
        case EFF_CODE_FIRE: 
            fireRoutine(lamp_state);
            break;
        case EFF_CODE_SPARKLES:
            sparklesRoutine(lamp_state);
            break;
        case EFF_CODE_MATRIX:
            matrixRoutine(lamp_state);
            break;
        case EFF_CODE_STARFALL:
            starfallRoutine(lamp_state);
            break;
        case EFF_CODE_SNAKE:
            snakeRoutine(lamp_state);
            break;
        case EFF_CODE_LAVA:
            lavaNoise(lamp_state);
            break;
        case EFF_CODE_CLOUD:
            cloudNoise(lamp_state);
            break;
        case EFF_CODE_ZEBRA:
            zebraNoise(lamp_state);
            break;
        case EFF_CODE_FOREST:
            forestNoise(lamp_state);
            break;
        case EFF_CODE_OCEAN:
            oceanNoise(lamp_state);
            break;
        case EFF_CODE_PLASMA:
            plasmaNoise(lamp_state);
            break;
        case EFF_CODE_PLUM:
            plumNoise(lamp_state);
            break;
        case EFF_CODE_RANDOM:
            randomNoise(lamp_state);
            break;
        case EFF_CODE_RAINBOW:
            rainbowNoise(lamp_state);
            break;
        case EFF_CODE_RAINBOW_STRIPE:
            rainbowStripeNoise(lamp_state);
            break;
    }
}

void convertRAW(the_lamp_state_t *lamp_state) {
    lamp_state->brightness_raw = (uint8_t)(lamp_state->brightness+1) >> 4;
    lamp_state->speed_raw      = (uint8_t)((512-lamp_state->speed) >> 4) +1;
    if (lamp_state->effect <= EFF_CODE_SNAKE )
        lamp_state->scale_raw  = (uint8_t)lamp_state->scale;
    else
        lamp_state->scale_raw  = (uint8_t)lamp_state->scale / 10;
    DPRINTLNF("convertRAW: done");
}

void updateMode(the_lamp_state_t *lamp_state) {
    lamp_state->loadingFlag = true;
    FastLED.clear();
    FastLED.show();

    DPRINTLNF("updateMode: done");
}

void setMode(the_lamp_state_t *lamp_state, uint8_t Value) {
    lamp_state->effect = Value;
    updateMode(lamp_state);

    DPRINTF("setMode: lamp_state->effect=");
    DPRINTLN(lamp_state->effect);
}

void changeMode(the_lamp_state_t *lamp_state, int8_t ChangeValue) {
    setMode(lamp_state, ChangeParameterValue(lamp_state->effect, ChangeValue, MODES_AMOUNT-1, false));

    DPRINTF("changeMode: lamp_state->effect=");
    DPRINTLN(lamp_state->effect);
}

void nextMode(the_lamp_state_t *lamp_state) {
    setMode(lamp_state, ChangeParameterValue(lamp_state->effect, +1, MODES_AMOUNT-1, false));

    DPRINTF("nextMode: lamp_state->effect=");
    DPRINTLN(lamp_state->effect);
}

void prevMode(the_lamp_state_t *lamp_state) {
    setMode(lamp_state, ChangeParameterValue(lamp_state->effect, -1, MODES_AMOUNT-1, false));

    DPRINTF("prevMode: lamp_state->effect=");
    DPRINTLN(lamp_state->effect);
}

void setBrightness(the_lamp_state_t *lamp_state, uint8_t Value) {
    lamp_state->brightness = Value;
    FastLED.setBrightness(Value);

    DPRINTF("SetBrightness: brightness=");
    DPRINTLN(FastLED.getBrightness());
}

void changeBrightness(the_lamp_state_t *lamp_state, int8_t ChangeValue) {
    uint8_t brightness = (uint8_t)ChangeParameterValue(FastLED.getBrightness(), ChangeValue);
    lamp_state->brightness = brightness;
    FastLED.setBrightness(brightness);

    DPRINTF("BrightnessChange: brightness=");
    DPRINTLN(brightness);
}

void setEffectsSpeed(the_lamp_state_t *lamp_state, uint32_t Value) {
    lamp_state->speed = Value;
    lamp_state->effectTimer->setInterval((uint32_t)Value);
    lamp_state->effectTimer->reset();

    DPRINTF("SetEffectsSpeed: interval=");
    DPRINTLN(Value);
}

void changeEffectsSpeed(the_lamp_state_t *lamp_state, int8_t ChangeValue) {
    uint16_t new_interval = ChangeParameterValue((uint16_t)lamp_state->effectTimer->getInterval(), ChangeValue, 1024);
    lamp_state->speed = new_interval;
    lamp_state->effectTimer->setInterval((uint32_t)new_interval);
    lamp_state->effectTimer->reset();

    DPRINTF("EffectsSpeedChange: interval=");
    DPRINTLN(new_interval);
}
