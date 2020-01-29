#ifndef __INC_MICHAEL_MINIMALTIMER_H
#define __INC_MICHAEL_MINIMALTIMER_H

#include <Arduino.h>

class M_MinimalTimer {
    public:
        M_MinimalTimer(uint32_t interval);      // Timer's constructor with interval setting
        void setInterval(uint32_t interval);    // Redefine of the timer setting
        uint32_t getInterval();                 // Get current timer's setting
        uint32_t getMillis();                   // Get current milliseconds counter (available and actual after 'isReady' function calling)
        boolean isReady();                      // Determines whether an interval has passed or not (return TRUE if yes) 
        void reset();                           // Reset current timer count 

    private:
        uint32_t _millis = 0;
        uint32_t _timer = 0;
        uint32_t _interval = 0;
};

#endif // __INC_MICHAEL_MINIMALTIMER_H