#pragma once

#include <Arduino.h>
#include <DHT.h>

/************************************/

struct SensorData
{
    float temperature;
    float humidity;
    bool isValid;

    const char *sensor;
    const char *error;
    bool isNewError;
};

/************************************/

class SensorManager
{
public:
    SensorManager(uint8_t dhtPin, uint8_t dhtType);

    void begin();
    SensorData readData();

private:
    DHT _dht;
    uint8_t _dhtType;
    bool _errorReported = false;

    const char *getSensorName();
};
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
