#include "WiFiManager.h"
#include "secrets.h"

#include <WiFi.h>

/************************************/

void WiFiManager::begin() //* Initialize the WiFi connection using the credentials defined in the Secrets.h file.
{
    WiFi.begin(
        Secrets::WIFI_SSID, //? Use the SSID defined in Secrets.h
        Secrets::WIFI_PASS);
}

/************************************/

void WiFiManager::update() //* Check the WiFi connection status and attempt to reconnect if disconnected.
{
    static unsigned long lastAttempt = 0;

    if (WiFi.status() == WL_CONNECTED)
        return;

    if (millis() - lastAttempt >= 5000)
    {
        lastAttempt = millis();

        WiFi.disconnect();
        WiFi.begin(
            Secrets::WIFI_SSID,
            Secrets::WIFI_PASS);
    }
}

/************************************/

bool WiFiManager::isConnected() //* Check if the WiFi is connected and return true if it is, false otherwise.
{
    return WiFi.status() == WL_CONNECTED;
}

/************************************/

IPAddress WiFiManager::getLocalIP() //* Return the local IP address assigned to the device by the WiFi network. If not connected, it will return an invalid IP address.
{
    return WiFi.localIP();
}
