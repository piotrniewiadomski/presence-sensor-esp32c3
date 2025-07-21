# Presence sensor using ESP32-C3 and HLK-LD2410S

## Overview
This project implements a presence detection system using the ESP32-C3 microcontroller interfaced with the HLK-LD2410S presence sensor over UART. The device connects to a MQTT broker (e.g., Home Assistant) to publish presence status updates.

## Power Management Strategy
* The ESP32-C3 runs in deep sleep power mode most of the time.
* The sensor is powered continuously to allow instant presence detection with no delays.
* The ESP32 wakes up from deep sleep immediately on presence status change detected from OT2 sensor pin, publishes the updated presence status over MQTT, then returns to the deep sleep awaiting the next OT2 output pin level change.

## Key Features
* UART communication with HLK-LD2410S sensor
* Wi-Fi management and connection handling
* MQTT client integration for publishing sensor data
* Modular component-based architecture for maintainability
* Built with ESP-IDF and developed using Visual Studio Code

## Set up
- Written, built and flashed with: ESP-IDF v5.5-rc1-dirty.
- Board used: ESP32-C3 Super Mini
- MQTT's and Wifi's congiruations set via: idf.py menuconfig
- When the sensor and the ESP32 are powered on, the sensor is send a command to automatically set the energy thresholds for all the gates and while this process takes place, one should keep any bodies or items, that should be later detected, away from the sensor's range.

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