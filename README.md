# Keyer Morse ESP32-S3

## Description

Projet de keyer pour saisir du Morse, basé sur un ESP32-S3 et destiné à être programmé avec PlatformIO sous VSCodium (et non VSCode).

## Matériel

- Boîtier imprimé en 3D et visserie.
- Deux contacteurs à palettes (*paddle*).
- Éventuellement une LED.
- Si possible, un micro-écran OLED SSD1306.
- **ESP32-S3-Zero** pour le prototypage.
- **ESP32-S3-WROOM WiFi/Bluetooth** utilisé pour le prototypage.
- **ESP32-S3-N16R8** comme cible finale.
- Une sortie TRS 3,5 mm pour le signal dih/dah.

## Fonctionnalités

- Saisie de Morse à l’aide de deux contacteurs à palettes.
- Programmation via PlatformIO sous VSCodium.
- Deux modes de fonctionnement : un mode configuration et un mode filaire normal.
- En mode filaire, comportement de clavier USB HID lorsque l’ESP32-S3 est branché en USB.
- En parallèle du HID USB, envoi d’un signal dih/dah sur une sortie TRS 3,5 mm branchée en parallèle des contacteurs paddles.

## Modes de fonctionnement

### 1. Mode configuration

L’interface utilisateur se fait via l’écran OLED s’il est présent. Sinon, le keyer fonctionne en mode point d’accès (*AP*) avec un serveur web proposant une page de configuration simple.

Le mode configuration est activé automatiquement si l’ESP32-S3 est branché et que l’utilisateur maintient les deux paddles appuyées simultanément pendant plus de 10 secondes.

### 2. Mode filaire (normal)

Lorsque l’ESP32-S3 est branché en USB, le keyer se comporte comme un clavier USB HID. Par défaut, il envoie le standard VBand avec les caractères `[` et `]`.

En parallèle du HID USB, un signal dih/dah est envoyé sur une sortie TRS 3,5 mm branchée en parallèle des contacteurs paddles. Cette sortie permet l’usage direct avec une radio, même si l’ESP32-S3 est débranché du USB.

## Architecture logicielle

- Le projet est programmé avec **PlatformIO** sous **VSCodium**.
- L’ESP32-S3 constitue le matériel de prototypage et la cible du keyer.
- Au démarrage, le maintien simultané des deux paddles pendant plus de 10 secondes, lorsque l’ESP32-S3 est branché, active automatiquement le mode configuration.
- Le mode configuration utilise l’interface OLED lorsqu’elle est présente ; à défaut, il utilise un mode AP et un serveur web avec une page de configuration simple.
- Le mode filaire utilise le HID USB pour envoyer les caractères configurés et transmet en parallèle le signal dih/dah sur la sortie TRS 3,5 mm.

## Limitations connues

Le mapping du clavier HID USB repose sur une table **ASCII → Usage ID** basée sur la disposition **QWERTY US**, fournie par la bibliothèque `USBHIDKeyboard` d’`arduino-esp32`. Si le système d’exploitation de l’utilisateur utilise une disposition de clavier différente, par exemple **AZERTY**, les caractères reçus peuvent être différents de ceux configurés.

Une solution envisagée consiste à envoyer directement des **Usage ID HID bruts**, indépendants du mapping du système d’exploitation.

## Licence

Ce projet est mis à disposition sous licence **Creative Commons CC BY-NC-SA 4.0** (*Attribution – Pas d’Utilisation Commerciale – Partage dans les Mêmes Conditions*).

Texte officiel de la licence : [CC BY-NC-SA 4.0 — texte en français](https://creativecommons.org/licenses/by-nc-sa/4.0/deed.fr)

Toute exploitation commerciale de ce projet ou de ses dérivés est interdite sans l’autorisation explicite de l’auteur.

## Avertissements

- Le mode configuration nécessite un écran OLED SSD1306 s’il est utilisé avec l’interface OLED ; en l’absence d’écran, il utilise le mode AP et le serveur web avec une page de configuration simple.
- Le mode filaire utilise l’USB HID lorsqu’il est branché en USB et transmet simultanément le signal dih/dah sur la sortie TRS 3,5 mm.
- La sortie TRS 3,5 mm est branchée en parallèle des contacteurs paddles pour permettre l’usage direct avec une radio, y compris lorsque l’ESP32-S3 est débranché du USB.
- Les caractères transmis par le HID USB peuvent varier selon la disposition du clavier configurée dans le système d’exploitation ; cette limitation est documentée dans la section « Limitations connues ».
