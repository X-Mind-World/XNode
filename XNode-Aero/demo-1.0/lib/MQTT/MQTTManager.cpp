#include "MQTTManager.h"
#include <ArduinoJson.h>
#include "secrets.h"
#include "configs.h"

/************************************/

MQTTManager::MQTTManager() //* Make sure to initialize the PubSubClient with the WiFiClient in the constructor initializer list
    : _mqttClient(_wifiClient)
{
}

/************************************/

void MQTTManager::begin() //* Initialize the MQTT client and set the server and callback function
{
    _mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

    _mqttClient.setCallback(
        [this](char *topic, byte *payload, unsigned int length)
        {
            callback(topic, payload, length);
        });
}

/************************************/

void MQTTManager::loop() //* Handle the MQTT client loop, reconnect if necessary.
{
    if (WiFi.status() != WL_CONNECTED)
        return;

    if (!_mqttClient.connected())
    {
        unsigned long now = millis();

        if (now - _lastReconnectAttempt >= MQTT_RECONNECT_INTERVAL)
        {
            _lastReconnectAttempt = now;
            connect();
        }
        return;
    }

    _mqttClient.loop(); //? Handle the MQTT client loop
}

/************************************/

bool MQTTManager::connect() //* Connect to the MQTT broker and publish the online status. Subscribe to the command topic if successful.
{
    snprintf(_statusTopic, sizeof(_statusTopic), "%s%s/status", MQTT_TOPIC_PREFIX, XNODE_ID); //? Format the status topic
    snprintf(_commandTopic, sizeof(_commandTopic), "%s%s/command", MQTT_TOPIC_PREFIX, XNODE_ID);
    snprintf(_telemetryTopic, sizeof(_telemetryTopic), "%s%s/telemetry", MQTT_TOPIC_PREFIX, XNODE_ID);
    snprintf(_errorTopic, sizeof(_errorTopic), "%s%s/error", MQTT_TOPIC_PREFIX, XNODE_ID);

    if (_mqttClient.connect( //? Connect to the MQTT broker with the specified client ID, username, password, and will message.
            XNODE_ID,        // Client ID
            Secrets::MQTT_USER,
            Secrets::MQTT_PASS,
            _statusTopic,
            1,
            true,
            "{\"state\":\"offline\",\"reason\":\"unexpected_disconnect\"}"))
    {
        _mqttClient.publish(
            _statusTopic,
            "{\"state\":\"online\"}",
            true);

        if (!subscribeTopics()){
            return false;
        }
        return true;
    }
    return false;
}

/************************************/

CommandType MQTTManager::parseCommandType(const String &command) //* Parse the command string and return the corresponding CommandType enum value.
{
    if (command == "set_led")
    {
        return CommandType::SetLed;
    }

    return CommandType::Unknown;
}

/************************************/

bool MQTTManager::subscribeTopics() //* Subscribe to the command topic and return true if successful.
{
    return _mqttClient.subscribe(_commandTopic);
}

/************************************/

void MQTTManager::callback(char *topic, byte *payload, unsigned int length) //* Callback function for handling incoming MQTT messages.
{
    if (String(topic) != _commandTopic)
    {
        return;
    }

    JsonDocument doc;
    DeserializationError error =
        deserializeJson(doc, payload, length);
    //? Check Command validity and parse the command type. If the command is valid, store it in _receivedCommand.
    if (error)
    {
        return;
    }

    if (!doc["command"].is<const char *>())
    {
        return;
    }

    String commandName = doc["command"].as<String>();

    CommandType commandType = parseCommandType(commandName);

    MQTTCommand newCommand;
    newCommand.valid = false;
    newCommand.type = commandType;

    switch (commandType) //* Validate the command based on its type and store the value if valid.
    {
    case CommandType::SetLed:
        // Validation for set_led
        {
            if (!doc["value"].is<int>())
            {
                return;
            }
            int value = doc["value"].as<int>();
            if (value != 0 && value != 1)
            {
                return;
            }
            newCommand.value = value;
            newCommand.valid = true;
            break;
        }

    case CommandType::Unknown:
    {
        return;
    }
    default:
    {
        return;
    }
    }

    if (newCommand.valid)
    {
        _receivedCommand = newCommand;
    }
}

/************************************/

MQTTCommand MQTTManager::readCommand() //* Read the last received command and return it.
{
    if (!_receivedCommand.valid)
    {
        return MQTTCommand{};
    }

    MQTTCommand command = _receivedCommand;

    _receivedCommand.valid = false;

    return command;
}

/************************************/

bool MQTTManager::publishSensorData(const SensorData &data) //* Publish the sensor data to the telemetry topic. If the data is invalid and new error information is available, publish the error information to the error topic as well.
{
    if(!_mqttClient.connected()){
        return false;
    }
    
    if (!data.isValid && data.isNewError)
    {
        JsonDocument errorDoc; //* Create a JSON document and populate it with the error information.

        errorDoc["error"] = data.error;
        errorDoc["sensor"] = data.sensor;

        char errorPayload[128];

        serializeJson(errorDoc, errorPayload, sizeof(errorPayload));

        _mqttClient.publish(
            _errorTopic,
            errorPayload);
    }

    JsonDocument doc; //* Create a JSON document and populate it with the sensor data.

    doc["temperature"] = data.temperature;
    doc["humidity"] = data.humidity;
    doc["valid"] = data.isValid;

    char payload[128];

    serializeJson(doc, payload, sizeof(payload));

    return _mqttClient.publish(
        _telemetryTopic,
        payload);
}

/************************************/

bool MQTTManager::isConnected() //* Check if the MQTT client is connected to the broker.
{
    return _mqttClient.connected();
}
