#include <Arduino.h>
#include "SensorManager.h"

constexpr uint8_t DHT_PIN = 5;

SensorManager sensors(DHT_PIN);

void setup()
{
    Serial.begin(115200);
    sensors.begin();
}

void loop()
{
    static unsigned long lastRead = 0;

    if (millis() - lastRead >= 5000)
    {
        lastRead = millis();

        SensorData data = sensors.readData();

        if (data.isValid)
        {
            static char buffer[100];
            Serial.print("Temperature: ");
            Serial.print(data.temperature);
            Serial.println(" °C");

            Serial.print("Humidity: ");
            Serial.print(data.humidity);
            Serial.println(" %");
        }
        else
        {
            Serial.println("Failed to read DHT11.");
        }
    }
}
