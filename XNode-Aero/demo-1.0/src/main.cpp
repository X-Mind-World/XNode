/**************************************************************
 _    _       _       _____      _  _  _             _     _
\ \  / /     (_)     (____ \    | || || |           | |   | |
 \ \/ / ____  _ ____  _   \ \   | || || | ___   ____| | _ | |
  )  ( |    \| |  _ \| |   | |  | ||_|| |/ _ \ / ___) |/ || |
 / /\ \| | | | | | | | |__/ /   | |___| | |_| | |   | ( (_| |
/_/  \_\_|_|_|_|_| |_|_____/     \______|\___/|_|   |_|\____|

***************************************************************
 * Project    : XNode-Aero - Modular IoT Project
 * Purpose    : Learning and Practicing Modular Architecture
 *
 * Note       : This project is being developed step by step.
 *              The code and related files will be continuously
 *              updated as new features and modules are added
 *              until the project reaches its final form.
 *
 ** Author    : XminD Team (education.xmindworld@gmail.com)
 * Date       : 2026-09
 *
 ** XminD Official Channels: https://linkshub.xmindworld.ir
 ***************************************************************/

#include <Arduino.h>
#include "SensorManager.h"
#include "WiFiManager.h"

constexpr uint8_t DHT_PIN = 5;

SensorManager sensors(DHT_PIN);
WiFiManager wifi;

void setup()
{
    Serial.begin(115200);
    sensors.begin();
    wifi.begin();
}

void loop()
{
    // Manage Network Connection:
    wifi.update();

    static bool lastWiFiState = false;

    bool currentWiFiState = wifi.isConnected();

    if (currentWiFiState != lastWiFiState)
    {
        lastWiFiState = currentWiFiState;

        if (currentWiFiState)
        {
            Serial.println("WiFi is Connected");
            Serial.print("IP: ");
            Serial.println(wifi.getLocalIP());
        }
        else
        {
            Serial.println("WiFi is Disconnected");
        }
    }

    // Read Sensor Data:
    static unsigned long lastRead = 0;

    if (millis() - lastRead >= 5000)
    {
        lastRead = millis();

        SensorData data = sensors.readData();

        if (data.isValid)
        {
            char buffer[128];
            snprintf(buffer, sizeof(buffer), "Temperature: %.2f °C\tHumidity: %.2f %%\n", data.temperature, data.humidity);
            Serial.print(buffer);
        }
        else
        {
            Serial.println("Failed to read DHT11.");
        }
    }
}
