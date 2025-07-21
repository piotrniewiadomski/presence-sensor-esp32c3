# Presence sensor based on ESP32-C3 & HLK-LD2410S

## Overview
This project implements a presence detection system using the ESP32-C3 microcontroller interfaced with the 24G ultra low-power radar sensing module HLK-LD2410S over UART. The device connects to a MQTT broker (e.g., Home Assistant) to publish presence status updates.

## Power Management Strategy
* The ESP32-C3 runs in deep sleep power mode most of the time.
* The module is powered continuously to allow instant presence detection with no delays.
* The ESP32 wakes up from deep sleep immediately on presence status change detected from OT2 module module pin, publishes the updated presence status over MQTT, then returns to the deep sleep awaiting the next OT2 pin level change.

## Key Features
* UART communication with HLK-LD2410S module.
* Wi-Fi management and connection handling.
* MQTT client integration for publishing module data.
* Modular component-based architecture for maintainability.
* Built with ESP-IDF and developed using Visual Studio Code.

## Set up
- Written, built and flashed with ESP-IDF v5.5-rc1-dirty and ESP-IDF VSCode extension.
- Board used is ESP32-C3 Super Mini.
- MQTT's and Wifi's configurations set via `idf.py menuconfig`
- When the module and the ESP32 are powered on, the module is send a command to automatically set the energy thresholds for all the gates and while this process takes place, one should keep any bodies or items, that should be later detected, away from the module's sensor range.

## Project Structure
    main  
    ├── CMakeLists.txt  
    └── main.c  
    components
    ├── app_logic
    │   ├── app_logic.cpp
    │   ├── app_logic.hpp
    │   └── CMakeLists.txt
    ├── mqtt_publisher
    │   ├── CMakeLists.txt
    │   ├── Kconfig.projbuild
    │   ├── mqtt_publisher.c
    │   └── mqtt_publisher.h
    ├── uart_hlk_ld2410s
    │   ├── CMakeLists.txt
    │   ├── uart_hlk_ld2410s.c
    │   └── uart_hlk_ld2410s.h
    └── wifi_manager
        ├── CMakeLists.txt
        ├── Kconfig.projbuild
        ├── wifi_service.c
        └── wifi_service.h 
    .devcontainer
    ├── devcontainer.json
    └── Dockerfile
    CMakeLists.txt  
    .gitattributes  
    .gitignore  
    README.md  
