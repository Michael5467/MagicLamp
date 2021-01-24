#include "web_functions.h"

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
#ifdef DEBUG_PRINT
    Serial.printf("webSocketEvent: num = %0x, type = %0x\r\n", num, type);
#endif

    switch (type)
    {
    case WStype_DISCONNECTED:
#ifdef DEBUG_PRINT
        Serial.printf("[%0x] Disconnected!\r\n", num);
#endif
        break;

    case WStype_CONNECTED:
    {
#ifdef DEBUG_PRINT
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%0x] Connected from %d.%d.%d.%d url: %s\r\n", num, ip[0], ip[1], ip[2], ip[3], payload);
#endif

        // send message to client
        webSocket.sendTXT(num, "Connected");

        // configSetup = readFile("config.json", 4096);

        // int effect = jsonReadtoInt(configSetup, "effect");
        // webSocket.sendTXT(num, String("effect") + effect);

        // int volume = jsonReadtoInt(configSetup, "volume");
        // webSocket.sendTXT(num, String("volume") + volume);

        // int speed = jsonReadtoInt(configSetup, "speed");
        // webSocket.sendTXT(num, String("speed") + speed);

        // int scale = jsonReadtoInt(configSetup, "scale");
        // webSocket.sendTXT(num, String("scale") + scale);
    }
    break;

    case WStype_TEXT:
#ifdef DEBUG_PRINT
        Serial.printf("[%0x] get Text: %s\r\n", num, payload);
#endif

        // if (strcmp(LEDON, (const char *)payload) == 0)
        // {
        //     writeLED(true);
        // }
        // else if (strcmp(LEDOFF, (const char *)payload) == 0)
        // {
        //     writeLED(false);
        // }

        // if (strcmp(LEDON2, (const char *)payload) == 0)
        // {
        //     writeLED2(true);
        // }
        // else if (strcmp(LEDON3, (const char *)payload) == 0)
        // {
        //     writeLED2(true);
        // }
        // else if (strcmp(LEDOFF3, (const char *)payload) == 0)
        // {
        //     writeLED2(false);
        // }
        // else if (strcmp(LEDOFF2, (const char *)payload) == 0)
        // {
        //     writeLED2(false);
        // }
        // else
        // {
        //     //  Serial.println("Unknown command");
        // }
        // // send data to all connected clients
        // webSocket.broadcastTXT(payload, length);
        break;

    case WStype_BIN:
#ifdef DEBUG_PRINT
        Serial.printf("[%0x] get binary length: %u\r\n", num, length);
#endif
        hexdump(payload, length);

        // // echo data back to browser
        // webSocket.sendBIN(num, payload, length);
        break;

    default:
#ifdef DEBUG_PRINT
    {
        const char *WStypeNames[] = {
            "WStype_ERROR",
            "WStype_DISCONNECTED",
            "WStype_CONNECTED",
            "WStype_TEXT",
            "WStype_BIN",
            "WStype_FRAGMENT_TEXT_START",
            "WStype_FRAGMENT_BIN_START",
            "WStype_FRAGMENT",
            "WStype_FRAGMENT_FIN",
            "WStype_PING",
            "WStype_PONG"
        };
        Serial.printf("[%0x] invalid WStype: %s\r\n", num, WStypeNames[type]);
        // Serial.printf("[%0x] invalid WStype: %u\r\n", num, type);
    }
#endif
    break;
    }
}
