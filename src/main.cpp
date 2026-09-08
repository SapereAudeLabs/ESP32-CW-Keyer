#include <Arduino.h>

#include "USB.h"
#include "USBHIDKeyboard.h"
#include <Adafruit_NeoPixel.h>

#include "Config.h"
#include "AccessPointMode.h"
#include "KeyerSettings.h"

USBHIDKeyboard Keyboard;

KeyerSettingsStore settingsStore;
AccessPointMode accessPointMode(settingsStore);

Adafruit_NeoPixel pixel(
    LED_PIXEL_COUNT,
    LED_PIN,
    NEO_GRB + NEO_KHZ800
);

struct Paddle
{
    uint8_t pin;
    char key;

    bool stableState;
    bool lastRawState;
    bool keyPressed;

    uint32_t lastChangeTime;
};

Paddle paddleLeft = {
    PADDLE_LEFT_PIN,
    DEFAULT_LEFT_PADDLE_CHARACTER,
    HIGH,
    HIGH,
    false,
    0
};

Paddle paddleRight = {
    PADDLE_RIGHT_PIN,
    DEFAULT_RIGHT_PADDLE_CHARACTER,
    HIGH,
    HIGH,
    false,
    0
};

bool ledActive = false;
uint32_t ledStartTime = 0;

bool configurationCombinationActive = false;
bool configurationModeRequested = false;
uint32_t configurationStartTime = 0;

void flashLed()
{
    pixel.setPixelColor(0, pixel.Color(0, 255, 0));
    pixel.show();

    ledActive = true;
    ledStartTime = millis();
}

void updateLed()
{
    if (ledActive &&
        millis() - ledStartTime >= LED_FLASH_TIME_MS)
    {
        pixel.setPixelColor(0, 0);
        pixel.show();
        ledActive = false;
    }
}

bool updatePaddle(Paddle& paddle)
{
    const bool rawState = digitalRead(paddle.pin);

    if (rawState != paddle.lastRawState)
    {
        paddle.lastRawState = rawState;
        paddle.lastChangeTime = millis();
    }

    if (millis() - paddle.lastChangeTime >= DEBOUNCE_TIME_MS &&
        rawState != paddle.stableState)
    {
        paddle.stableState = rawState;
        return true;
    }

    return false;
}

void updateConfigurationHold()
{
    const bool bothPressed =
        paddleLeft.stableState == LOW &&
        paddleRight.stableState == LOW;

    if (!bothPressed)
    {
        configurationCombinationActive = false;
        configurationStartTime = 0;
        return;
    }

    if (!configurationCombinationActive)
    {
        configurationCombinationActive = true;
        configurationStartTime = millis();

        Serial.println(
            "Appui simultane detecte : "
            "maintien requis pendant 10 secondes."
        );

        return;
    }

    if (!configurationModeRequested &&
        millis() - configurationStartTime >= CONFIG_HOLD_TIME_MS)
    {
        configurationModeRequested = true;

        Serial.println(
            "Activation du mode configuration."
        );
    }
}

void processPaddle(Paddle& paddle)
{
    const bool pressed = paddle.stableState == LOW;

    if (pressed && !paddle.keyPressed)
    {
        Keyboard.press(paddle.key);
        paddle.keyPressed = true;

        flashLed();

        Serial.print("Appui paddle : ");
        Serial.println(paddle.key);
    }
    else if (!pressed && paddle.keyPressed)
    {
        Keyboard.release(paddle.key);
        paddle.keyPressed = false;

        Serial.print("Relachement paddle : ");
        Serial.println(paddle.key);
    }
}

void initializePaddle(Paddle& paddle)
{
    paddle.stableState = digitalRead(paddle.pin);
    paddle.lastRawState = paddle.stableState;
    paddle.lastChangeTime = millis();
}

void releasePaddleKey(Paddle& paddle)
{
    if (paddle.keyPressed)
    {
        Keyboard.release(paddle.key);
        paddle.keyPressed = false;
    }
}

void setup()
{
    Serial.begin(115200);
    delay(500);

    pinMode(PADDLE_LEFT_PIN, INPUT_PULLUP);
    pinMode(PADDLE_RIGHT_PIN, INPUT_PULLUP);

    pixel.begin();
    pixel.show();

    initializePaddle(paddleLeft);
    initializePaddle(paddleRight);

    settingsStore.load();

    const KeyerSettings& settings = settingsStore.get();

    paddleLeft.key = settings.leftPaddleCharacter;
    paddleRight.key = settings.rightPaddleCharacter;

    Serial.print("Left Paddle character : ");
    Serial.println(paddleLeft.key);

    Serial.print("Right Paddle character : ");
    Serial.println(paddleRight.key);

    Keyboard.begin();
    USB.begin();

    Serial.println("Keyer HID demarre");
}

void loop()
{
    if (accessPointMode.isRunning())
    {
        accessPointMode.loop();
        delay(2);
        return;
    }

    updateLed();

    // Chaque paddle est lu une seule fois par cycle
    updatePaddle(paddleLeft);
    updatePaddle(paddleRight);

    updateConfigurationHold();

    if (configurationModeRequested)
    {
        releasePaddleKey(paddleLeft);
        releasePaddleKey(paddleRight);

        if (!accessPointMode.begin())
        {
            Serial.println(
                "Erreur : impossible de demarrer le mode AP."
            );
        }
        else
        {
            Serial.println(
                "Connecter un appareil au reseau Wi-Fi : "
                "Morse-Keyer-Setup"
            );
        }

        // Évite de rappeler begin() à chaque tour de boucle
        configurationModeRequested = false;

        return;
    }

    processPaddle(paddleLeft);
    processPaddle(paddleRight);

    delay(1);
}