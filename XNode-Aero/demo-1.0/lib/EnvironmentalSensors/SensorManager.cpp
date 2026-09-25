#include "SensorManager.h"

SensorManager::SensorManager(uint8_t dhtPin)
    : _dht(dhtPin, DHT11)
{
}

void SensorManager::begin()
{
    _dht.begin();
}

SensorData SensorManager::readData()
{
    SensorData data;

    data.temperature = _dht.readTemperature();
    data.humidity = _dht.readHumidity();

    data.isValid =
        !isnan(data.temperature) &&
        !isnan(data.humidity);

    return data;
}
