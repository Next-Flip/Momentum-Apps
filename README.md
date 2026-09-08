# ⚡️ IR Jammer for Flipper Zero

An advanced, high-performance Infrared Jamming application for Flipper Zero. Optimized for late 2026 Flipper SDK versions and specifically redesigned to unlock the full potential of powerful **external 6W IR Blasters**.

## ⚠️ Hardware Safety Advice
* When **EXTERNAL** mode is active, the app pushes massive current to Pin A7 and enables 5V OTG power. 
* High-power 6W IR Blasters will heat up rapidly. To protect your IR LEDs from thermal degradation, **do not run active jamming for more than 10-15 seconds continuously**.
* Always toggle status to `PAUSED` before hot-plugging or removing the external module from the GPIO header.

## ✨ About the Project
This is my very first custom application built for Flipper Zero. I am still learning how to work with GPIO, power lines, and the Flipper screen. (This project was created and refined with the assistance of AI, which helped fix compiler bugs and organize the clean architecture).
