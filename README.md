# Presence sensor using ESP32-C3 and HLK-LD2410S

This project implements a presence detection system using the ESP32-C3 microcontroller with the HLK-LD2410S presence sensor connected via UART. The firmware initializes Wi-Fi, configures the sensor, keeps the  and publishes presence status updates to an MQTT broker (e.g., Home Assistant) — only sending messages when the sensor state changes to optimize network traffic.

**Power Management Strategy**
* The ESP32-C3 runs in light sleep mode with the Wi-Fi connection maintained to balance low power consumption and responsiveness.
* The sensor is powered continuously to allow instant presence detection with no delays.
* The ESP32 wakes immediately on UART data received from the sensor, publishes the updated presence status over MQTT, then returns to light sleep awaiting the next event.

**Key Features**
* UART communication with HLK-LD2410S sensor
* Wi-Fi management and connection handling
* MQTT client integration for publishing sensor data
* Modular component-based architecture for maintainability
* Built with ESP-IDF and developed using Visual Studio Code

**Project Structure**
main  
├── CMakeLists.txt  
└── main.c  
components  
├── app_logic  
│   ├── app_logic.cpp  
│   ├── app_logic.hpp  
│   └── CMakeLists.txt  
├── mqtt_client  
│   ├── CMakeLists.txt  
│   ├── mqtt_publisher.c  
│   └── mqtt_publisher.h  
├── uart_hlk-ld2410s  
│   ├── CMakeLists.txt  
│   ├── uart_hlk-ld2410s.c  
│   └── uart_hlk-ld2410s.h  
└── wifi_manager  
    ├── CMakeLists.txt  
    ├── wifi_service.c  
    └── wifi_service.h  
CMakeLists.txt  
.gitattributes  
.gitignore  
README.md  