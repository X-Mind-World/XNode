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
#include "MQTTManager.h"


constexpr uint8_t DHT_PIN = 5;
constexpr uint8_t DHT_TYPE = DHT11;
constexpr uint8_t LED_PIN = 2;

const char* dhtName =
    (DHT_TYPE == DHT11) ? "DHT11" :
    (DHT_TYPE == DHT22) ? "DHT22" :
                          "Unknown";

SensorManager sensors(DHT_PIN, DHT_TYPE);
WiFiManager wifi;
MQTTManager mqtt;

void setup()
{
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    sensors.begin();
    wifi.begin();
    mqtt.begin();
}

void loop()
{
    wifi.update();
    mqtt.loop();

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

MQTTCommand command = mqtt.readCommand();

if (command.valid)
{
    switch (command.type)
    {
    case CommandType::SetLed:
        Serial.print("Received SetLed command. Value: ");
        Serial.println(command.value);
        digitalWrite(LED_PIN, command.value);
        break;

    default:
        break;
    }
}

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
            Serial.printf("Failed to read %s.\n", dhtName);
        }
        mqtt.publishSensorData(data);
    }
}

