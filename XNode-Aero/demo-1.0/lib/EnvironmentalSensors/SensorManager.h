#pragma once

#include <Arduino.h>
#include <DHT.h>

struct SensorData
{
    float temperature;
    float humidity;
    bool isValid;
};

class SensorManager
{
public:
    SensorManager(uint8_t dhtPin);

    void begin();
    SensorData readData();

private:
    DHT _dht;
};
