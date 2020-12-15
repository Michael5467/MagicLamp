#include "json_functions.h"

void debugPrintVariant(const JsonVariant& object, const char* key)
{
    JsonVariant variant = object.getMember("version");
    if (variant.isNull())
    {
        // Serial.print("debugPrintVariant: ");
        // Serial.print(key);
        // Serial.println("is missing!");
        Serial.printf("debugPrintVariant: [%s] is missing!\r\n", key);
    }
    else
    {
        Serial.printf("debugPrintVariant: [%s] = [%s]\r\n", key, variant.as<const char*>());
    }
}

// Loads the configuration from file
void loadConfiguration(const char *filename, the_lamp_state_t &config)
{
    // Open file for reading
    File file = LittleFS.open(filename, "r");

    // Allocate a temporary JsonDocument
    // Don't forget to change the capacity to match your requirements.
    // Use arduinojson.org/v6/assistant to compute the capacity.
    StaticJsonDocument<512> doc;

    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, file);
    if (error)
    {
        Serial.print(F("Failed to read file, using default configuration: "));
        Serial.println(error.c_str());
        return;
    }   

    JsonObject root = doc.as<JsonObject>();

    debugPrintVariant(root, "product");
    debugPrintVariant(root, "version");
    debugPrintVariant(root, "releaseDate");

    for (int i = 0; i < 7; i++)
    {
        awake_alarm_arr[i].state = false;
    }
    JsonArray array = root.getMember("awake_alarm");
    if (!array.isNull())
    {
        Serial.println("=======================================");
        Serial.printf("array.size()=%d", array.size());

        for (JsonVariant var : array)
        {
            Serial.println(var.as<const char *>());
        }

        // Serial.println(".......................................");

        // // https://github.com/nopnop2002/Arduino-Json-Example/blob/master/ArduinoJson-Example2/ArduinoJson-Example2.ino
        // for (size_t i = 0; i < array.size(); i++)
        // {
        //     JsonVariant variant = array[i];
        //     // parseVariant(variant);
        //     // Serial.println();
        //     // TODO: write parser

        //     Serial.printf("array[%d] = %d, %0d:%0d ", i, variant["state"], variant["hour"], variant["min"]);
        // }
    }

    // https://forum.arduino.cc/index.php?topic=441243.0
    // // now iterate the actions array
    // Serial.println("Iterate actions ...");
    // JsonArray &actions = json["actions"];
    // for (auto dataobj : actions)
    // {
    //     Serial.println(dataobj.key);
    // }

    // Close the file (Curiously, File's destructor doesn't close the file)
    file.close();
}

// Saves the configuration to file
void saveConfiguration(const char *filename, const the_lamp_state_t &config)
{
    // // Delete existing file, otherwise the configuration is appended to the file
    // SD.remove(filename);

    // // Open file for writing
    // File file = SD.open(filename, FILE_WRITE);
    // if (!file)
    // {
    //     Serial.println(F("Failed to create file"));
    //     return;
    // }

    // // Allocate a temporary JsonDocument
    // // Don't forget to change the capacity to match your requirements.
    // // Use arduinojson.org/assistant to compute the capacity.
    // StaticJsonDocument<256> doc;

    // // Set the values in the document
    // doc["hostname"] = config.hostname;
    // doc["port"] = config.port;

    // // Serialize JSON to file
    // if (serializeJson(doc, file) == 0)
    // {
    //     Serial.println(F("Failed to write to file"));
    // }

    // // Close the file
    // file.close();
}

// Prints the content of a file to the Serial
void printFile(const char *filename)
{
    // Open file for reading
    File file = LittleFS.open(filename, "r");

    if (!file)
    {
        Serial.println(F("Failed to read file"));
        return;
    }

    // Extract each characters by one by one
    while (file.available())
    {
        Serial.print((char)file.read());
    }
    Serial.println();

    // Close the file
    file.close();
}
