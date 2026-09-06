#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include "KeyerSettings.h"

class AccessPointMode
{
public:
    AccessPointMode(KeyerSettingsStore& settingsStore);

    bool begin();
    void loop();

    bool isRunning() const;

private:
    void handleRoot();
    void handleSave();
    void handleNotFound();

    String makeConfigurationPage(
        const String& message = String()
    ) const;

    String htmlEscape(const String& value) const;

    KeyerSettingsStore& settingsStore;
    WebServer server;
    bool running;
};