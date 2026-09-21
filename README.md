# ESP32-CW-Keyer

ESP32-CW-Keyer is a simple implementation for building a USB and TRS compatible CW Keyer. 

This is a programmable alternative to the official VBand dongle, as well as Chinese copies that often needs tweaking at boot to be handle to use Vband mapping.

Fully compatible on computers, mobilec and tablets as it behave like a keyboard.

In USB mode, when plugged in, it is recognized as a HID keyboard and transmits by default the Vband standard "[" and "]". 
The TRS mode, on the other hand, is fully analog and does not require any USB connection — only TRS, like most dual-paddle contact keyers. 

Holding both paddles pressed for more than 5 seconds switches the unit into AP mode, allowing you to connect via WiFi to the ESP32 to access a configuration page. 

This project contains the platformIO sources, as well as (coming soon) STL files and instructions to physically build the unit.

ESP32-S3 are required to handle HID keyboard support.
