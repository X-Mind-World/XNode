#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "SensorManager.h"

/************************************/

enum class CommandType //* Define the types of commands that can be received via MQTT.
{
    Unknown,
    SetLed
};

/************************************/

struct MQTTCommand //* Structure to hold the details of a received MQTT command.
{
    bool valid = false;
    CommandType type = CommandType::Unknown;
    int value = 0;
};

/************************************/

class MQTTManager //* Class to manage MQTT connections, publishing sensor data, and receiving commands.
{
public:
    MQTTManager(); //? Constructor to initialize the MQTTManager instance.

    void begin();
    void loop();
    bool isConnected();

    bool publishSensorData(const SensorData &data);

    MQTTCommand readCommand();

private:
    bool connect();
    bool subscribeTopics();

    void callback(char *topic, byte *payload, unsigned int length);

    CommandType parseCommandType(const String &command);

    WiFiClient _wifiClient;
    PubSubClient _mqttClient;

    MQTTCommand _receivedCommand;

    unsigned long _lastReconnectAttempt = 0;
    char _statusTopic[100];
    char _commandTopic[100];
    char _telemetryTopic[100];
    char _errorTopic[100];
};