#pragma once

#include <Arduino.h>

struct KeyerSettings
{
    char leftPaddleCharacter;
    char rightPaddleCharacter;
};

class KeyerSettingsStore
{
public:
    void load();
    bool save();

    const KeyerSettings& get() const;

    bool setCharacters(char leftCharacter, char rightCharacter);

private:
    KeyerSettings settings;
};