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

    date_time->synchronized = true;
    date_time->local_millis = millis();
    date_time->local_time   = currentLocalTime;
    date_time->day          = convert_to_ISO8601(dayOfWeek(currentLocalTime)); // convert day of week into the standard russian form
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
        DPRINTLN("Internet connection is broken.");
        return false;
    }
    else {
        DPRINTLN("Internet connection established!");
        return true;
    }
}
