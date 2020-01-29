#include <Arduino.h>

#include <NTPClient.h>
#include <Timezone.h>
#include <TimeLib.h>

#include "project_config.h"
#include "functions.h"

#ifdef SUMMER_WINTER_TIME
TimeChangeRule summerTime = { SUMMER_TIMEZONE_NAME, SUMMER_WEEK_NUM, SUMMER_WEEKDAY, SUMMER_MONTH, SUMMER_HOUR, SUMMER_OFFSET };
TimeChangeRule winterTime = { WINTER_TIMEZONE_NAME, WINTER_WEEK_NUM, WINTER_WEEKDAY, WINTER_MONTH, WINTER_HOUR, WINTER_OFFSET };
Timezone localTimeZone(summerTime, winterTime);
#else
TimeChangeRule localTime = { LOCAL_TIMEZONE_NAME, LOCAL_WEEK_NUM, LOCAL_WEEKDAY, LOCAL_MONTH, LOCAL_HOUR, LOCAL_OFFSET };
Timezone localTimeZone(localTime);
#endif

boolean NTP_Synchronization(NTPClient *timeClient, local_date_time_t *date_time) {
    if (!CheckInternetAccess()) {
        date_time->synchronized = false;
        return false;
    }

    if (!timeClient->update()) {
        date_time->synchronized = false;
        return false;
    }

    time_t currentLocalTime = localTimeZone.toLocal(timeClient->getEpochTime());

    // The Time library uses day of the week in the European numbering (days numbered from Sunday). 
    // So we have to convert it into the standard russian form (also compatible with ISO-8601).
    uint8_t thisDay = dayOfWeek(currentLocalTime);
    if (thisDay == 1) {
        thisDay = 8;
    }
    thisDay -= 2;
    
    date_time->synchronized = true;
    date_time->local_millis = millis();
    date_time->local_time   = currentLocalTime;
    date_time->day          = thisDay;
    date_time->hour         = hour(currentLocalTime);
    date_time->minute       = minute(currentLocalTime);
    date_time->second       = second(currentLocalTime);

    printDateTimeStruct(date_time);
    return true;
}

boolean CheckInternetAccess()
{
    IPAddress ntpServerIp = {0, 0, 0, 0};

    WiFi.hostByName(NTP_ADDRESS, ntpServerIp, NTP_RESOLVE_TIMEOUT);
    if (ntpServerIp[0] <= 0) {
#ifdef DEBUG_PRINT
        Serial.println("Internet connection is broken.");
#endif
        return false;
    }
    else {
#ifdef DEBUG_PRINT
        Serial.println("Internet connection established!");
#endif
        return true;
    }
}
