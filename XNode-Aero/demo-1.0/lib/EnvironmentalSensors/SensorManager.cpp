#include "SensorManager.h"

/************************************/

SensorManager::SensorManager(uint8_t dhtPin, uint8_t dhtType) //* Initialize the SensorManager with the specified DHT pin and type.
    : _dht(dhtPin, dhtType),
      _dhtType(dhtType)
{
}

/************************************/

void SensorManager::begin() //* Initialize the DHT sensor by calling its begin() method.
{
    _dht.begin();
}

/************************************/

const char *SensorManager::getSensorName() //* Return the name of the DHT sensor based on its type.
{
    switch (_dhtType)
    {
    case DHT11:
        return "DHT11";

    case DHT22:
        return "DHT22";

    default:
        return "Unknown";
    }
}

/************************************/

SensorData SensorManager::readData() //* Read the temperature and humidity data from the DHT sensor. Return a SensorData structure containing the readings and their validity.
{
    SensorData data;

    data.temperature = _dht.readTemperature();
    data.humidity = _dht.readHumidity();

    data.isValid =
        !isnan(data.temperature) &&
        !isnan(data.humidity);

    if (data.isValid)
    {
        data.sensor = nullptr;
        data.error = nullptr; //?   Clear error message if data is valid.
        data.isNewError = false;

        _errorReported = false;
    }
    else
    {
        data.temperature = 0;
        data.humidity = 0;
        data.sensor = getSensorName();
        data.error = "Sensor Unavailable";
        data.isNewError = !_errorReported;

        _errorReported = true;
    }

    return data;
}
