
#include "TimeManager.h"

#include <WiFi.h>
#include <esp_sntp.h>//* for NTP synchronization

/************************************/
volatile bool TimeManager::_isSynced = false; //? Static member variable to track if the system time has been synchronized with an NTP server. Initialized to false.

void TimeManager::onTimeSync(struct timeval *tv){
    (void)tv;
    _isSynced = true;
}

bool TimeManager::begin(){  //* Initialize the TimeManager and start NTP synchronization. Returns true if the initialization was successful, false otherwise.
        if (WiFi.status() != WL_CONNECTED)
        return false;
sntp_set_time_sync_notification_cb(onTimeSync);
    configTime(
        0,
        0,
        "pool.ntp.org",
        "time.nist.gov",
        "time.google.com");

        return true;
}

/************************************/

bool TimeManager::isSynced() const //* Check if the system time has been synchronized with an NTP server. Returns true if synchronized, false otherwise.
{
    return _isSynced;
}

/************************************/

time_t TimeManager::getUnixTimestamp() const //* Get the current Unix timestamp.
{
    if (!isSynced())
        return 0;

    time_t now;
    time(&now);

    return now;
}
