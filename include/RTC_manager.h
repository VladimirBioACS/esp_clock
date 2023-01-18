#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include <WiFi.h>
#include "time.h"

class RTC_manager
{

private:
    struct tm RTC_tm;
    
    const char* ntpServer = "0.ua.pool.ntp.org";
    const long  gmtOffset_sec = 0;
    const int   daylightOffset_sec = 3600;

public:
    void getLocalTime(void);
};

#endif