# FlipWiFi

FlipWiFi is the companion app for the popular FlipperHTTP flash, originally introduced in the https://github.com/jblanked/WebCrawler-FlipperZero/tree/main/assets/FlipperHTTP. It allows you to scan and save WiFi networks for use across all FlipperHTTP apps.

## Requirements

- WiFi Dev Board or Raspberry Pi Pico W for Flipper Zero with FlipperHTTP Flash: https://github.com/jblanked/FlipperHTTP
- WiFi Access Point


## Features

- **Scan**: Discover nearby WiFi networks and add them to your list.
- **Saved Access Points**: View your saved networks, manually add new ones, or configure the WiFi network to be used across all FlipperHTTP apps.

## Setup

FlipWiFi automatically allocates the necessary resources and initializes settings upon launch. If WiFi settings have been previously configured, they are loaded automatically for easy access. You can also edit the list of WiFi settings by downloading and modifying the "wifi_list.txt" file located in the "/SD/apps_data/flip_wifi/" directory. To use the app:

1. **Flash the WiFi Dev Board**: Follow the instructions to flash the WiFi Dev Board with FlipperHTTP: https://github.com/jblanked/WebCrawler-FlipperZero/tree/main/assets/FlipperHTTP
2. **Install the App**: Download FlipWiFi from the App Store.
3. **Launch FlipWiFi**: Open the app on your Flipper Zero.
4. Connect, review, and save WiFi networks.