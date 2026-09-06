#pragma once

#include <Arduino.h>

constexpr uint8_t PADDLE_LEFT_PIN  = 17;
constexpr uint8_t PADDLE_RIGHT_PIN = 18;

constexpr char DEFAULT_LEFT_PADDLE_CHARACTER  = ']';
constexpr char DEFAULT_RIGHT_PADDLE_CHARACTER = '[';

constexpr uint32_t DEBOUNCE_TIME_MS = 30;
constexpr uint32_t CONFIG_HOLD_TIME_MS = 5000;

constexpr char AP_SSID[] = "Morse-Keyer-Setup";
constexpr uint8_t AP_CHANNEL = 1;

#ifndef LED_BUILTIN
#define LED_BUILTIN 48
#endif

constexpr uint8_t LED_PIN = LED_BUILTIN;
constexpr uint32_t LED_FLASH_TIME_MS = 50;