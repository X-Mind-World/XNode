#pragma once

#include <Arduino.h>

class WiFiManager
{
public:
    void begin();
    void update();
    bool isConnected();
    IPAddress getLocalIP();
} ; 
