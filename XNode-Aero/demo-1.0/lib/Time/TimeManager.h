
#pragma once

#include <Arduino.h>
#include <time.h>
#include <sys/time.h> // *[NEW] for time management
/************************************/

class TimeManager
{
public:

    // TimeManager();
    bool begin();
    void update();

    bool isSynced() const;
    time_t getUnixTimestamp() const;
// *NEW ⇩
private:

    static volatile bool _isSynced;
    static void onTimeSync(struct timeval *tv);

};
