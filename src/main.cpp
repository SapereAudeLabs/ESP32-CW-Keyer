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


bool configurationLedState = false;
uint32_t configurationLedLastChange = 0;
bool configurationCombinationActive = false;
bool configurationModeRequested = false;
uint32_t configurationStartTime = 0;

void setLedColor(uint8_t red, uint8_t green, uint8_t blue)
{
    const uint8_t brightnessPercent =
        settingsStore.get().ledBrightnessPercent;

    const uint8_t brightness = map(
        brightnessPercent,
        0,
        100,
        0,
        255
    );

    pixel.setBrightness(brightness);

    pixel.setPixelColor(
        0,
        pixel.Color(red, green, blue)
    );

    pixel.show();
}

void updateNormalLed()
{
    const KeyerSettings& settings = settingsStore.get();

    if (!settings.ledEnabled)
    {
        setLedColor(0, 0, 0);
        return;
    }

    const bool leftPressed =
        paddleLeft.stableState == LOW;

    const bool rightPressed =
        paddleRight.stableState == LOW;

    if (leftPressed && rightPressed)
    {
        // Deux paddles : vert
        setLedColor(0, 255, 0);
    }
    else if (leftPressed)
    {
        // Paddle gauche : bleu
        setLedColor(0, 0, 255);
    }
    else if (rightPressed)
    {
        // Paddle droit : jaune
        setLedColor(255, 255, 0);
    }
    else
    {
        // État normal : violet
        setLedColor(255, 0, 255);
    }
}

void updateConfigurationLed()
{
    const uint32_t now = millis();

    if (now - configurationLedLastChange >= 1000)
    {
        configurationLedLastChange = now;
        configurationLedState = !configurationLedState;

        if (configurationLedState)
        {
            setLedColor(255, 0, 0);
        }
        else
        {
            setLedColor(0, 0, 0);
        }
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
        updateConfigurationLed();
        accessPointMode.loop();
        delay(2);
        return;
    }

    // Chaque paddle est lu une seule fois par cycle
    updatePaddle(paddleLeft);
    updatePaddle(paddleRight);

    updateNormalLed();

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