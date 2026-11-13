#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "SensorManager.h"

/************************************/

class MQTTManager //* Class to manage MQTT connections, publishing sensor data, and receiving commands.
{
public:
    MQTTManager(); //? Constructor to initialize the MQTTManager instance.

    void begin();
    void loop();
    bool isConnected();

    bool publishSensorData(const SensorData &data, time_t timestamp);


private:
    bool connect();
    bool subscribeTopics();

    void callback(char *topic, byte *payload, unsigned int length);

    WiFiClient _wifiClient;
    PubSubClient _mqttClient;

    bool _subscribed = false;
    unsigned long _lastSubTry = 0;
    static constexpr unsigned long SUBSCRIBE_RETRY_INTERVAL = 3000;

    bool _subsStatusReported = false;
    bool _lastSubsReported = false;


    unsigned long _lastReconnectAttempt = 0;
    char _statusTopic[100];
    char _commandTopic[100];
    char _telemetryTopic[100];
    char _errorTopic[100];

/*
* Build the queue for error events to be sent to the MQTT broker. This is useful for storing error events when the MQTT connection is down, and sending them once the connection is re-established. [Experimental]
*/
    static constexpr size_t ERROR_QUEUE_SIZE = 4;
    static constexpr size_t ERROR_PAYLOAD_SIZE = 128;

    struct PendingErrorEvent
    {
        char payload[ERROR_PAYLOAD_SIZE];
    };

    PendingErrorEvent _errorQueue[ERROR_QUEUE_SIZE]{};
    size_t _errorQueueCount = 0;
    size_t _droppedEvents = 0;
    size_t _front = 0;
    size_t _rear = 0;

    bool enqueueErrorEvent(const SensorData &data, time_t timestamp);

    void publishPendingErrorEvents();
};
