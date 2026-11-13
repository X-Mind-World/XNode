

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

    snprintf(_statusTopic, sizeof(_statusTopic), "%s%s/status", MQTT_TOPIC_PREFIX, XNODE_ID); //? Format the status topic
    snprintf(_commandTopic, sizeof(_commandTopic), "%s%s/command", MQTT_TOPIC_PREFIX, XNODE_ID);
    snprintf(_telemetryTopic, sizeof(_telemetryTopic), "%s%s/telemetry", MQTT_TOPIC_PREFIX, XNODE_ID);
    snprintf(_errorTopic, sizeof(_errorTopic), "%s%s/error", MQTT_TOPIC_PREFIX, XNODE_ID);

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

    if(!_subscribed){
        unsigned long now = millis();
        if(now - _lastSubTry > SUBSCRIBE_RETRY_INTERVAL){
            _lastSubTry = now;
            _subscribed = subscribeTopics();
            publishSubscriptionStatus();
        }
    }
}

/************************************/

bool MQTTManager::connect() //* Connect to the MQTT broker and publish the online status. Subscribe to the command topic if successful.
{

    if (_mqttClient.connect( //? Connect to the MQTT broker with the specified client ID, username, password, and will message.
            XNODE_ID,        // Client ID
            Secrets::MQTT_USER,
            Secrets::MQTT_PASS,
            _statusTopic,
            1,
            true,
            "{\"state\":\"offline\",\"reason\":\"unexpected_disconnect\"}"))
    {
        _subsStatusReported = false; // *Reset the subscription status reported flag
        _lastSubsReported = false;    // *Reset the last subscription status reported flag
        _lastSubTry = millis();   
        _subscribed = subscribeTopics();
        publishSubscriptionStatus(); // *Publish the online status to the status topic with the retained flag set to true. This will ensure that the last known status is available to new subscribers.
        return true;
    }
    return false;
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

bool MQTTManager::publishSensorData(const SensorData &data, time_t timestamp) //* Publish the sensor data to the telemetry topic. If the data is invalid and new error information is available, publish the error information to the error topic as well.
{

    if (data.isNewError || data.isRecovered)
    {
        if (!enqueueErrorEvent(data, timestamp))
        {
            Serial.println("Failed to enqueue sensor event.");
        }
    }

    if (!_mqttClient.connected())
    {
        return false;
    }

    publishPendingErrorEvents();

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

/***********************************/

bool MQTTManager::enqueueErrorEvent(const SensorData &data, time_t timestamp) //* Enqueue an error event to be sent to the MQTT broker. If the queue is full, the oldest event will be dropped to make room for the new event. Returns true if the event was successfully enqueued, false otherwise.
{
 
    JsonDocument doc;

    doc["sensor"] = data.sensor;
    doc["state"] = data.state;

    if (data.isNewError)
    {
        doc["error"] = data.error;
    }

    doc["timestamp"] = static_cast<int64_t>(timestamp);

    char payload[ERROR_PAYLOAD_SIZE];

    size_t length = serializeJson(
        doc,
        payload,
        sizeof(payload));

    if (length == 0 || length >= sizeof(payload))
    {
        return false;
    }

    if (_errorQueueCount == ERROR_QUEUE_SIZE)
    {
        _front = (_front + 1) % ERROR_QUEUE_SIZE;
        --_errorQueueCount;
        ++_droppedEvents;
    } 

        PendingErrorEvent &event = _errorQueue[_rear];
        
        memcpy(event.payload, payload, length + 1);
        _rear = (_rear + 1) % ERROR_QUEUE_SIZE;

    ++_errorQueueCount;
    Serial.printf(
        "Event queued. Queue size: %u, Dropped: %u\n",
        static_cast<unsigned>(_errorQueueCount),
        static_cast<unsigned>(_droppedEvents));
    return true;
}

/******************************************/

void MQTTManager::publishPendingErrorEvents() //* Publish all pending error events in the queue to the MQTT broker. If the publish fails, the event will remain in the queue for future attempts. This function is called during the main loop when the MQTT client is connected.
{
    while (_mqttClient.connected() && _errorQueueCount > 0)
    {
        PendingErrorEvent &event = _errorQueue[_front];

        bool published = _mqttClient.publish(
            _errorTopic,
            event.payload,
            true);

        if (!published)
        {
            break;
        }

    _front = (_front + 1) % ERROR_QUEUE_SIZE;

        --_errorQueueCount;
    }
}
