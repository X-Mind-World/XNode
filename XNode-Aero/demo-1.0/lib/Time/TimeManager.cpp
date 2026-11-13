
#include "TimeManager.h"

#include <WiFi.h>
// #include <sys/time.h>
#include <esp_sntp.h>//*[NEW] for NTP synchronization

/************************************/
// *NEW
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
// ******************
// namespace
// {
//     constexpr time_t VALID_TIME_THRESHOLD = 1704067200;
// }

/************************************/

// void TimeManager::begin()
// {
//     if (WiFi.status() != WL_CONNECTED)
//         return;

//     configTime(
//         0,
//         0,
//         "pool.ntp.org",
//         "time.nist.gov",
//         "time.google.com");
// }

/************************************/

void TimeManager::update()
{
    // NTP synchronization is asynchronous.
    // No waiting or blocking is required here.
}

/************************************/

bool TimeManager::isSynced() const //* Check if the system time has been synchronized with an NTP server. Returns true if synchronized, false otherwise.
{
    // time_t now;
    // time(&now);

    // return now >= VALID_TIME_THRESHOLD;
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
