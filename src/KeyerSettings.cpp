#include "KeyerSettings.h"

#include <Preferences.h>

#include "Config.h"

namespace
{
    constexpr char NAMESPACE_NAME[] = "keyer";

    constexpr char LEFT_KEY[] = "left";
    constexpr char RIGHT_KEY[] = "right";
    constexpr char LED_KEY[] = "led";
    constexpr char LED_BRIGHTNESS_KEY[] = "brightness";
}

KeyerSettingsStore::KeyerSettingsStore()
    : settings{
          DEFAULT_LEFT_PADDLE_CHARACTER,
          DEFAULT_RIGHT_PADDLE_CHARACTER,
          DEFAULT_LED_ENABLED,
          DEFAULT_LED_BRIGHTNESS_PERCENT
      }
{
}

void KeyerSettingsStore::load()
{
    settings.leftPaddleCharacter =
        DEFAULT_LEFT_PADDLE_CHARACTER;

    settings.rightPaddleCharacter =
        DEFAULT_RIGHT_PADDLE_CHARACTER;

    settings.ledEnabled =
        DEFAULT_LED_ENABLED;

    settings.ledBrightnessPercent =
        DEFAULT_LED_BRIGHTNESS_PERCENT;

    Preferences preferences;

    if (!preferences.begin(NAMESPACE_NAME, true))
    {
        return;
    }

    settings.leftPaddleCharacter =
        preferences.getChar(
            LEFT_KEY,
            DEFAULT_LEFT_PADDLE_CHARACTER
        );

    settings.rightPaddleCharacter =
        preferences.getChar(
            RIGHT_KEY,
            DEFAULT_RIGHT_PADDLE_CHARACTER
        );

    settings.ledEnabled =
        preferences.getBool(
            LED_KEY,
            DEFAULT_LED_ENABLED
        );

    settings.ledBrightnessPercent =
        preferences.getUChar(
            LED_BRIGHTNESS_KEY,
            DEFAULT_LED_BRIGHTNESS_PERCENT
        );

    preferences.end();
}

bool KeyerSettingsStore::save()
{
    Preferences preferences;

    if (!preferences.begin(NAMESPACE_NAME, false))
    {
        return false;
    }

    preferences.putChar(
        LEFT_KEY,
        settings.leftPaddleCharacter
    );

    preferences.putChar(
        RIGHT_KEY,
        settings.rightPaddleCharacter
    );

    preferences.putBool(
        LED_KEY,
        settings.ledEnabled
    );

    preferences.putUChar(
        LED_BRIGHTNESS_KEY,
        settings.ledBrightnessPercent
    );

    preferences.end();

    return true;
}

const KeyerSettings& KeyerSettingsStore::get() const
{
    return settings;
}

bool KeyerSettingsStore::setCharacters(
    char leftCharacter,
    char rightCharacter
)
{
    if (leftCharacter == '\0' ||
        rightCharacter == '\0')
    {
        return false;
    }

    settings.leftPaddleCharacter = leftCharacter;
    settings.rightPaddleCharacter = rightCharacter;

    return true;
}

void KeyerSettingsStore::setLedEnabled(bool enabled)
{
    settings.ledEnabled = enabled;
}

void KeyerSettingsStore::setLedBrightnessPercent(
    uint8_t brightness
)
{
    settings.ledBrightnessPercent =
        constrain(brightness, 0, 100);
}