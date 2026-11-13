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
 * Date       : 2026-11
 *
 ** XminD Official Channels: https://linkshub.xmindworld.ir
 ***************************************************************/

#include <Arduino.h>
#include "SensorManager.h"
#include "WiFiManager.h"
#include "MQTTManager.h"
/**********/
#include "TimeManager.h"
/**********/

constexpr uint8_t DHT_PIN = 5;
constexpr uint8_t DHT_TYPE = DHT11;

const char *dhtName =
    (DHT_TYPE == DHT11) ? "DHT11" : (DHT_TYPE == DHT22) ? "DHT22"
                                                        : "Unknown";

SensorManager sensors(DHT_PIN, DHT_TYPE);
WiFiManager wifi;
MQTTManager mqtt;
TimeManager timeManager;

void checkConnectionStatus()
{
    static bool lastWiFiState = false;
    static bool lastMqttState = false;

    bool currentWiFiState = wifi.isConnected();
    bool currentMqttState = mqtt.isConnected();

    if (currentWiFiState != lastWiFiState)
    {
        lastWiFiState = currentWiFiState;

        if (currentWiFiState)
        {
            Serial.println("WiFi is Connected ✅");
            Serial.print("IP: ");
            Serial.println(wifi.getLocalIP());
        }
        else
        {
            Serial.println("WiFi is Disconnected");
        }
    }

    if (currentMqttState != lastMqttState)
    {
        lastMqttState = currentMqttState;

        if (currentMqttState)
        {
            Serial.println("Connected to MQTT broker. ✅");
        }
        else
        {
            Serial.println("Attempting to connect to MQTT broker...");
        }
    }
}

void setup()
{
    Serial.begin(115200);

    sensors.begin();
    wifi.begin();
    mqtt.begin();
}

void loop()
{
    wifi.update();

/*****************/

static bool timeSyncStarted = false;
if (wifi.isConnected() && !timeSyncStarted)
{

    timeSyncStarted = timeManager.begin();

    if(timeSyncStarted){
        Serial.println("NTP time synchronization started.");
    }
}

static bool lastTimeSyncState = false;
bool currentTimeSyncState = timeManager.isSynced();

if (currentTimeSyncState != lastTimeSyncState)
{
    lastTimeSyncState = currentTimeSyncState;

    if (currentTimeSyncState)
    {
        Serial.printf(
            "System time synchronized. Unix timestamp: %lld\n",
            static_cast<long long>(timeManager.getUnixTimestamp()));
    }
    else
    {
        Serial.println("System time is not synchronized.");
    }
}
    
/*****************/

    mqtt.loop();

     checkConnectionStatus();

    static unsigned long lastRead = 0;

    if (millis() - lastRead >= 5000)
    {
        lastRead = millis();

        SensorData data = sensors.readData();
        time_t timestamp = timeManager.getUnixTimestamp();

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
        mqtt.publishSensorData(data, timestamp);
    }
}

