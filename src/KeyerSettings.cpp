#include "KeyerSettings.h"

#include <Preferences.h>

#include "Config.h"

namespace
{
    constexpr char NAMESPACE_NAME[] = "keyer";
    constexpr char LEFT_KEY[] = "left";
    constexpr char RIGHT_KEY[] = "right";
}

void KeyerSettingsStore::load()
{
    Preferences preferences;

    if (!preferences.begin(NAMESPACE_NAME, true))
    {
        settings.leftPaddleCharacter =
            DEFAULT_LEFT_PADDLE_CHARACTER;

        settings.rightPaddleCharacter =
            DEFAULT_RIGHT_PADDLE_CHARACTER;

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

    preferences.end();
}

bool KeyerSettingsStore::save()
{
    Preferences preferences;

    if (!preferences.begin(NAMESPACE_NAME, false))
    {
        return false;
    }

    const size_t leftWritten =
        preferences.putChar(
            LEFT_KEY,
            settings.leftPaddleCharacter
        );

    const size_t rightWritten =
        preferences.putChar(
            RIGHT_KEY,
            settings.rightPaddleCharacter
        );

    preferences.end();

    return leftWritten == sizeof(char) &&
           rightWritten == sizeof(char);
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
    if (leftCharacter == '\0' || rightCharacter == '\0')
    {
        return false;
    }

    settings.leftPaddleCharacter = leftCharacter;
    settings.rightPaddleCharacter = rightCharacter;

    return true;
}