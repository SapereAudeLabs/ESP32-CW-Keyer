#pragma once

#include <Arduino.h>

constexpr uint8_t PADDLE_LEFT_PIN  = 6;
constexpr uint8_t PADDLE_RIGHT_PIN = 7;

constexpr char DEFAULT_LEFT_PADDLE_CHARACTER  = ']';
constexpr char DEFAULT_RIGHT_PADDLE_CHARACTER = '[';

constexpr uint32_t DEBOUNCE_TIME_MS = 30;
constexpr uint32_t CONFIG_HOLD_TIME_MS = 5000;

constexpr char AP_SSID[] = "Morse-Keyer-Setup";
constexpr uint8_t AP_CHANNEL = 1;

constexpr uint8_t LED_PIN = 21;
constexpr uint8_t LED_PIXEL_COUNT = 1;
constexpr uint32_t LED_FLASH_TIME_MS = 50;

constexpr bool DEFAULT_LED_ENABLED = true;
constexpr uint8_t DEFAULT_LED_BRIGHTNESS_PERCENT = 50;
