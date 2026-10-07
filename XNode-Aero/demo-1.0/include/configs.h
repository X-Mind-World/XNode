#pragma once

/* Define the MQTT broker address, port, and other related constants. */

constexpr char MQTT_BROKER[] = "YOUR_BROKER_ADDRESS"; //* Define the MQTT broker address.
constexpr uint16_t MQTT_PORT = 1883;
constexpr char XNODE_ID[] = "xnode-aero-01";
constexpr char MQTT_TOPIC_PREFIX[] = "xmind/xnode/";
constexpr unsigned long MQTT_RECONNECT_INTERVAL = 5000;
