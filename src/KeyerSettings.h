#pragma once

#include <Arduino.h>

struct KeyerSettings
{
    char leftPaddleCharacter;
    char rightPaddleCharacter;
    bool ledEnabled;
    uint8_t ledBrightnessPercent;
};

class KeyerSettingsStore
{
public:
    KeyerSettingsStore();

    void load();
    bool save();

    const KeyerSettings& get() const;

    bool setCharacters(
        char leftCharacter,
        char rightCharacter
    );

    void setLedEnabled(bool enabled);
    void setLedBrightnessPercent(uint8_t brightness);

private:
    KeyerSettings settings;
};