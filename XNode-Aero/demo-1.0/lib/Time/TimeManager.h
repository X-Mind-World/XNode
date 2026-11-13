
#pragma once

#include <Arduino.h>
#include <time.h>
#include <sys/time.h> // * for time management
/************************************/

class TimeManager
{
public:

    bool begin();
    void update();

    bool isSynced() const;
    time_t getUnixTimestamp() const;

private:

    static volatile bool _isSynced;
    static void onTimeSync(struct timeval *tv);

};
