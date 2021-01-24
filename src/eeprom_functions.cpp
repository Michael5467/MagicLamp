#include "eeprom_function.h"

uint8_t eeprom_read_1B(uint16_t address)
{
    uint8_t data;
    uint8_t *p = (uint8_t *)&data;
    *p = EEPROM.read(address);
    return data;
}

uint16_t eeprom_read_2B(uint16_t address)
{
    uint16_t data;
    uint8_t *p = (uint8_t *)&data;
    *p = EEPROM.read(address);
    *(p + 1) = EEPROM.read(address + 1);
    return data;
}

uint32_t eeprom_read_4B(uint16_t address)
{
    uint32_t data;
    uint8_t *p = (uint8_t *)&data;
    *p = EEPROM.read(address);
    *(p + 1) = EEPROM.read(address + 1);
    *(p + 2) = EEPROM.read(address + 2);
    *(p + 3) = EEPROM.read(address + 3);
    return data;
}

bool eeprom_write_1B(uint16_t address, int8_t data)
{
    uint8_t *p = (uint8_t *)&data;
    EEPROM.write(address, *p);
    return EEPROM.commit();
}

bool eeprom_write_2B(uint16_t address, int16_t data)
{
    uint8_t *p = (uint8_t *)&data;
    EEPROM.write(address, *p);
    EEPROM.write(address + 1, *(p + 1));
    return EEPROM.commit();
}

bool eeprom_write_4B(uint16_t address, int32_t data)
{
    uint8_t *p = (uint8_t *)&data;
    EEPROM.write(address, *p);
    EEPROM.write(address + 1, *(p + 1));
    EEPROM.write(address + 2, *(p + 2));
    EEPROM.write(address + 3, *(p + 3));
    return EEPROM.commit();
}

String eeprom_read_string(uint16_t startAdress, uint16_t endAdress)
{
    String s = "";
    // Serial.println();
    for (uint16_t i = startAdress; i <= endAdress; i++)
    {
        char c = char(EEPROM.read(i));
        // Serial.print(c);
        s += String(c);
    }
    // Serial.println();
    return s;
}


// #include <EEPROM.h>

// // EEPROM MAP  addresses
// size_t EPROM_MEMORY_SIZE= 512;
// int stmodeAddressStart = 0; int stmodeAddressEnd = 0;                               // size 1  byte  which stores 1 bit (0/1)
// int serverAccessSSIDAddressStart = 1; int serverAccessSSIDAddressEnd = 34;          // size 34 bytes of char includes \0
// int serverAccessPasswordAddressStart = 35; int serverAccessPasswordAddressEnd = 68; // size 34 bytes of char includes \0
// int serverAccessIPAddressStart = 69; int serverAccessIPAddressEnd = 72;             // size 4  bytes , each byte is int in range of 0-255
// int serverAccessPortAddressStart = 73; int serverAccessPortAddressEnd = 74;         // size 2  bytes , 2 bytes represent int int range of 0-65535


// void setup() {
// 	Serial.begin(115200);

// 	char Server_router_ssid[] = "A1111111111111111111111111111111A\0";	
// 	char Server_router_password[] = "A1121112111111111111111111211111A\0";
// 	String ip = "233.14.180.100";
// 	int port = 5000;

// 	Serial.println("");
// 	Serial.println("Start Write");
// 	EEPROM.begin(EPROM_MEMORY_SIZE);
// 	EEPROM.put(serverAccessSSIDAddressStart, Server_router_ssid);
// 	delay(500);
// 	Serial.println("Write 1 done");
// 	EEPROM.put(serverAccessPasswordAddressStart, Server_router_password);
// 	delay(500);
// 	Serial.println("Write 2 Done");
	
// 	int ipArr[4];
// 	ConvertStringIpToIntArray(ip,ipArr);
// 	for (int i = 0; i < 4; i++) {
// 		EEPROM.write(serverAccessIPAddressStart+i,ipArr[i]);
// 		delay(100);
// 	}

// 	Serial.println("Write 3 Done");

// 	EEPROM.put(serverAccessPortAddressStart, port);
// 	delay(100);

// 	Serial.println("Write 4 Done");

// 	EEPROM.commit();
// 	Serial.println("commited");


// 	Serial.println("READ");
// 	EEPROM.begin(EPROM_MEMORY_SIZE);
// 	char res[34];
// 	EEPROM.get(serverAccessSSIDAddressStart, res);
// 	Serial.println(res);
	
// 	char res2[34];
// 	EEPROM.get(serverAccessPasswordAddressStart, res2);
// 	Serial.println(res2);

// 	int res3[4];
// 	for (int i = 0; i < 4; i++) {
// 		res3[i] = EEPROM.read(serverAccessIPAddressStart+i);
// 		Serial.println(res3[i]);
// 	}

// 	int res4;
// 	EEPROM.get(serverAccessPortAddressStart, res4);
// 	Serial.println(res4);

// }

// void loop() {

// }

// void ConvertStringIpToIntArray(String inputIP,int outPutIP[]) {
// 	int startDotIndex = -1;
// 	int endDotIndex = inputIP.indexOf('.');
// 	int ipSize = 4;

// 	for (int i = 0; i < ipSize; i++) {

// 		String number = inputIP.substring(startDotIndex + 1, endDotIndex);
// 		outPutIP[i] = number.toInt();
// 		startDotIndex = endDotIndex;
// 		endDotIndex = inputIP.indexOf('.', endDotIndex + 1);
// 	}
// }
