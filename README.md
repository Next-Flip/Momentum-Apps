# ⚡️ IR Jammer for Flipper Zero

An advanced, high-performance Infrared Jamming application for Flipper Zero. Optimized for late 2026 Flipper SDK versions and specifically redesigned to unlock the full potential of powerful **external 6W IR Blasters**.

## 📦 How to Build and Install

The best way to install the app without facing any **API version mismatch** errors is compiling it locally via `uFBT` against your current firmware build.

1. Connect your Flipper Zero to your PC via USB.
2. Clone this repository and open a terminal inside the project directory:
   ```bash
   git clone https://github.com
   cd YOUR_REPO_NAME
   ```
3. Build and launch the application directly onto your device:
   ```bash
   ufbt launch
   ```

## ⚠️ Hardware Safety Advice
* When **EXTERNAL** mode is active, the app pushes massive current to Pin A7 and enables 5V OTG power. 
* High-power 6W IR Blasters will heat up rapidly. To protect your IR LEDs from thermal degradation, **do not run active jamming for more than 10-15 seconds continuously**.
* Always toggle status to `PAUSED` before hot-plugging or removing the external module from the GPIO header.
