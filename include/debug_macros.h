// DebugMacros.h

// Example of use:
// #define DEBUG_PRINT                 // <-----<<<< this line must appear before the include line
// #include "debug_macros.h"
//

//If you comment the line:  #define DEBUG_PRINT
//The Macro lines are defined as blank, thus would be ignored by the compiler.
//If DEBUG_PRINT is NOT commented, these macros will be included in the sketch.
// examples:
// This  converts to         >>>>----->         This OR a Blank Line.
//--------------------------------------------------------------------------------------------
// DPRINTLN("Testing123");   >>>>----->     Serial.println("Testing123");
// DPRINTLN(0xC0FFEEul,DEC); >>>>----->     Serial.println(0xC0FFEEul,DEC);
// DPRINTLN(12648430ul,HEX); >>>------>     Serial.println(12648430ul,HEX);
// DPRINTLNF("This text came from flash");  Serial.println(F("This text came from flash"));
// DPRINT(myVariable);       >>>>----->     Serial.print(myVariable);
// DELAY(100);               >>>>----->     delay(100);
// SERIALBEGIN(9600);        >>>>----->     Serial.begin(9600);
// PINMODE(13,OUTPUT);       >>>>----->     pinMode(13,OUTPUT);
// TOGGLEd13;                >>>>----->     PINB = 0x20;  // D13 Toggle,for UNO ONLY
//
// Also, this works  #define INFO(...)  { Console->printf("INFO: "); Console->printf(__VA_ARGS__); }   >>>--->   where {} allows multiple lines of code.
// See: http://forum.arduino.cc/index.php?topic=511393.msg3485833#new

#define DEBUG_PRINT

//examples:
//#define DPRINT(args...)  Serial.print(args)             //OR use the following syntax:

#ifdef DEBUG_PRINT

#define SERIALBEGIN(...)   Serial.begin(__VA_ARGS__);

#define DPRINT(...)        Serial.print(__VA_ARGS__);
#define DPRINTLN(...)      Serial.println(__VA_ARGS__);
#define DPRINTF(...)       Serial.print(F(__VA_ARGS__));
#define DPRINTLNF(...)     Serial.println(F(__VA_ARGS__)); //Printing text using the F macro
#define DPRINT_FULL(...)   Serial.print(F(#__VA_ARGS__" = ")); Serial.print(__VA_ARGS__); Serial.print(F("; "));
#define DPRINTLN_FULL(...) Serial.print(F(#__VA_ARGS__" = ")); Serial.println(__VA_ARGS__);

#else

#define SERIALBEGIN(...)  

#define DPRINT(...)      
#define DPRINTLN(...)    
#define DPRINTF(...)      
#define DPRINTLNF(...)    
#define DPRINT_FULL(...)    
#define DPRINTLN_FULL(...)  

#endif

#define SERIALWRITE(...)   Serial.write(__VA_ARGS__)

#define DELAY(...)         delay(__VA_ARGS__)             // Delay function
#define PINMODE(...)       pinMode(__VA_ARGS__)
#define TOGGLEd13          PINB = 0x20                    //For the UNO only
