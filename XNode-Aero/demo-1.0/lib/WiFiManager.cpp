#include "WiFiManager.h"
#include "secrets.h"

#include <WiFi.h>

void WiFiManager::begin()
{
    WiFi.begin(
        Secrets::WIFI_SSID,
        Secrets::WIFI_PASS
    );
}

void WiFiManager::update()
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
            Secrets::WIFI_PASS
        );
    }
}

bool WiFiManager::isConnected()
{
    return WiFi.status() == WL_CONNECTED;

}

IPAddress WiFiManager::getLocalIP()
{
    return WiFi.localIP();
}