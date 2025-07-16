# Presence sensor using ESP32-C3 and HLK-LD2410S

## Overview

This project implements a presence detection system using the ESP32-C3 microcontroller interfaced with the HLK-LD2410S presence sensor over UART. The device connects to a MQTT broker (e.g., Home Assistant) to publish presence status updates.

## Power Management Strategy
* The ESP32-C3 runs in normal mode with minimal modem power mode to maintain the Wi-Fi connection and save power consumption.
* The sensor is powered continuously to allow instant presence detection with no delays.
* The ESP32 leaves the minimal modem power mode immediately on presence status change detected from data received via UART from the sensor, publishes the updated presence status over MQTT, then returns to the minimal modem power mode awaiting the next change.

## Key Features
* UART communication with HLK-LD2410S sensor
* Wi-Fi management and connection handling
* MQTT client integration for publishing sensor data
* Modular component-based architecture for maintainability
* Built with ESP-IDF and developed using Visual Studio Code

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
    CMakeLists.txt  
    .gitattributes  
    .gitignore  
    README.md  