/**************************************************************
 _    _       _       _____      _  _  _             _     _
\ \  / /     (_)     (____ \    | || || |           | |   | |
 \ \/ / ____  _ ____  _   \ \   | || || | ___   ____| | _ | |
  )  ( |    \| |  _ \| |   | |  | ||_|| |/ _ \ / ___) |/ || |
 / /\ \| | | | | | | | |__/ /   | |___| | |_| | |   | ( (_| |
/_/  \_\_|_|_|_|_| |_|_____/     \______|\___/|_|   |_|\____|

***************************************************************
 * Project    : XNode-Aero - Modular Sensor Management
 * Purpose    : Sensor Data Acquisition & Modular Architecture
 *              - Reading temperature and humidity from DHT11
 *              - Separating sensor logic from the main program
 *              - Managing sensor initialization and data reading
 *              - Returning structured sensor data
 *              - Keeping the main application logic clean and simple
 *
 * Note       : This project is being developed step by step.
 *              The code and related files will be continuously
 *              completed and updated as new features and modules
 *              are added until the project reaches its final form.
 *
 ** Author    : XminD Team (education.xmindworld@gmail.com)
 * Date       : 2026-09
 *
 ** XminD Official Channels: https://linkshub.xmindworld.ir
 ***************************************************************/

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
