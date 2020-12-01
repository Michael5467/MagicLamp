#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include "functions.h"

void changePower(the_lamp_state_t *lamp_state) {
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

// int32_t parse_get_request(the_lamp_state_t *lamp_state, get_command_t *command) {
//     register uint32_t temp_var;

//     DPRINT("parse_get_request> {");
//     DPRINT((char *)command);
//     DPRINTLN("}");

//     command->split_2 = 0;

//     if ( strcmp(command->type, "STS") == 0 ) {
//         DPRINT("> STATUS ");

//         // Status format:
//         // state (power); effect (mode); brightness; speed; scale
//         String status_str = "";
//         if (lamp_state->state) {
//             status_str += "ON;";
//         }
//         else {
//             status_str += "OFF;"
//         }
//         status_str += int2st

//         return 0;
//     }

//     return 0;
// }

int32_t parse_request(the_lamp_state_t *lamp_state, get_command_t *command/*, String *response*/) {
    register uint32_t temp_var;

    DPRINT("parse_request> {");
    DPRINT((char *)command);
    DPRINTLN("}");

    command->split_2 = 0;

    if ( strcmp(command->type, "CNN") == 0 ) {
        // response="Connection: done!";
        DPRINTLN("> CONNECT ");
        return 0;
    }
    else if ( strcmp(command->type, "STS") == 0 ) {
        // response=lamp_state_2_string(lamp_state);
        DPRINTLN("> STATUS ");
        return 1;
    }
    else if ( strcmp(command->type, "PWR") == 0 ) {
        if (command->param[0] == '0') {
            DPRINTLN("> POWER OFF");
            lamp_state->state = false;
        }
        else if (command->param[0] == '1') {
            DPRINTLN("> POWER ON");
            lamp_state->state = true;
        }
        changePower(lamp_state);
        return 0;
    }
    else if ( strcmp(command->type, "DBG") == 0 ) {
        DPRINTLN("> DEBUG ");
        lamp_state->debug = !(lamp_state->debug);
        return 0;
    }
    else if ( strcmp(command->type, "STP") == 0 ) {
        DPRINTLN("> DEBUG ");
        lamp_state->step = true;
        return 0;
    }
    else if ( strcmp(command->type, "MOD") == 0 ) {
        temp_var = str2int(command->param, 3);
        DPRINT("> MODE ");
        DPRINTLN(temp_var);
        setMode(lamp_state, temp_var);
        return 0;
    }
    else if ( strcmp(command->type, "BRI") == 0 ) {
        temp_var = str2int(command->param, 3);
        DPRINT("> BRIGHTNESS ");
        DPRINTLN(temp_var);
        lamp_state->brightness_raw = temp_var;
        setBrightness(lamp_state, (temp_var<<4)-1);
        return 0;
    }
    else if ( strcmp(command->type, "SPD") == 0 ) {
        temp_var = str2int(command->param, 4);
        DPRINT("> SPEED ");
        DPRINTLN(temp_var);
        lamp_state->speed_raw = temp_var;
        setEffectsSpeed(lamp_state, (512-((temp_var-1)<<4)));
        return 0;
    }
    else if ( strcmp(command->type, "DEN") == 0 ) {
        temp_var = str2int(command->param, 3);
        DPRINT("> DENSITY ");
        DPRINTLN(temp_var);
        lamp_state->scale_raw = temp_var;
        switch (lamp_state->effect) {
            case (EFF_CODE_SNOW):
            case (EFF_CODE_BALLS):
            case (EFF_CODE_FIRE):
            case (EFF_CODE_SPARKLES):
            case (EFF_CODE_MATRIX):
                break;
            case (EFF_CODE_STARFALL):
                temp_var = 5 * temp_var;
                break;
            case (EFF_CODE_SNAKE):
                break;
            case (EFF_CODE_LAVA):
            case (EFF_CODE_CLOUD):
            case (EFF_CODE_ZEBRA):
            case (EFF_CODE_FOREST):
            case (EFF_CODE_OCEAN):
            case (EFF_CODE_PLASMA):
            case (EFF_CODE_PLUM):
            case (EFF_CODE_RANDOM):
            case (EFF_CODE_RAINBOW):
            case (EFF_CODE_RAINBOW_STRIPE):
            	temp_var = 10 * temp_var;
                break;
        }
        lamp_state->scale = temp_var;
        updateMode(lamp_state);
        return 0;
    }
    return -1;
}

void ServerLoop(WiFiServer *server, the_lamp_state_t *lamp_state) {
    String header = ""; // Variable to store the HTTP request

    WiFiClient client = server->available();        // Listen for incoming clients
    if (client) {                                   // If a new client connects,
        DPRINTLN("New Client.");                    // print a message out in the serial port
        String currentLine = "";                    // make a String to hold incoming data from the client
        while (client.connected()) {                // loop while the client's connected
            if (client.available()) {               // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                SERIALWRITE(c);                     // print it out the serial monitor
                header += c;
                if (c == '\n') {                    // if the byte is a newline character
                    DPRINT("> currentLine = {");
                    DPRINT(currentLine);
                    DPRINTLN("}");
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        char header_buf[256];
                        header.toCharArray(header_buf, 256); 
                        String response = "";
                        const char *ps;

                        switch (parse_request(lamp_state, (get_command_t *)header_buf)) {
                            case 0:
                                DPRINTLN("Connection: done!");
                                client.println("Connection: done!");
                                break;
                            case 1:
                                response=lamp_state_2_string(lamp_state);
                                ps = response.c_str();
                                DPRINTLN(ps);
                                client.println(ps);
                                break;
                            default:
                                break;
                        }

                        // The HTTP response ends with another blank line
                        client.println();
                        // Break out of the while loop
                        break;
                    } 
                    else { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                } 
                else {
                    if (c != '\r') {  // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                    }
                }
            }
        }
        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        DPRINTLN("Client disconnected.");
        DPRINTLN("");
        randomSeed(micros());
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
            // starfallRoutine(lamp_state);
            starfallRoutine_new(lamp_state);
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
        case EFF_DAWN:
            Dawn(lamp_state);
            break;
    }
}

void updateMode(the_lamp_state_t *lamp_state) {
    lamp_state->loadingFlag = true;
    FastLED.clear();
    FastLED.show();
}

void setMode(the_lamp_state_t *lamp_state, uint8_t Value) {
    lamp_state->effect = Value;
    updateMode(lamp_state);

    DPRINT("setMode: lamp_state->effect=");
    DPRINTLN(lamp_state->effect);
}

void changeMode(the_lamp_state_t *lamp_state, int8_t ChangeValue) {
    setMode(lamp_state, ChangeParameterValue(lamp_state->effect, ChangeValue, MODES_AMOUNT-1, false));

    DPRINT("changeMode: lamp_state->effect=");
    DPRINTLN(lamp_state->effect);
}

void nextMode(the_lamp_state_t *lamp_state) {
    setMode(lamp_state, ChangeParameterValue(lamp_state->effect, +1, MODES_AMOUNT-1, false));

    DPRINT("nextMode: lamp_state->effect=");
    DPRINTLN(lamp_state->effect);
}

void prevMode(the_lamp_state_t *lamp_state) {
    setMode(lamp_state, ChangeParameterValue(lamp_state->effect, -1, MODES_AMOUNT-1, false));

    DPRINT("prevMode: lamp_state->effect=");
    DPRINTLN(lamp_state->effect);
}

void setBrightness(the_lamp_state_t *lamp_state, uint8_t Value) {
    lamp_state->brightness = Value;
    FastLED.setBrightness(Value);

    DPRINT("SetBrightness: brightness=");
    DPRINTLN(FastLED.getBrightness());
}

void changeBrightness(the_lamp_state_t *lamp_state, int8_t ChangeValue) {
    uint8_t brightness = (uint8_t)ChangeParameterValue(FastLED.getBrightness(), ChangeValue);
    lamp_state->brightness = brightness;
    FastLED.setBrightness(brightness);

    DPRINT("BrightnessChange: brightness=");
    DPRINTLN(brightness);
}

void setEffectsSpeed(the_lamp_state_t *lamp_state, uint32_t Value) {
    lamp_state->speed = Value;
    lamp_state->effectTimer->setInterval((uint32_t)Value);
    lamp_state->effectTimer->reset();

    DPRINT("SetEffectsSpeed: interval=");
    DPRINTLN(Value);
}

void changeEffectsSpeed(the_lamp_state_t *lamp_state, int8_t ChangeValue) {
    uint16_t new_interval = ChangeParameterValue((uint16_t)lamp_state->effectTimer->getInterval(), ChangeValue, 1024);
    lamp_state->speed = new_interval;
    lamp_state->effectTimer->setInterval((uint32_t)new_interval);
    lamp_state->effectTimer->reset();

    DPRINT("EffectsSpeedChange: interval=");
    DPRINTLN(new_interval);
}
