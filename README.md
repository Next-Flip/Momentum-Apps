# ⚡️ IR Jammer (Bruce Architecture) for Flipper Zero

An advanced, high-performance Infrared Jamming application for Flipper Zero, ported from the famous ESP32 **Bruce Firmware**. Optimized for late 2026 Flipper SDK versions and specifically redesigned to unlock the full potential of powerful **external 6W IR Blasters** without any system freezes.

## 🚀 Key Features

* **True Pulse Modulation:** Reimplemented Bruce's exact 38 kHz burst-jamming techniques at the hardware register level using ultra-fast GPIO bit-banging (`furi_hal_gpio_write`).
* **Zero Freezes:** No continuous heavy PWM reinits inside microsecond loops—preventing FreeRTOS scheduler lockups and button unresponsiveness.
* **100% Blaster Power:** Delivers maximum peak current to external MOSFET drivers. IR LEDs glow at their true physical limit.
* **Dual Output Support:** Easily switch between **INTERNAL** Flipper IR LED and **EXTERNAL** 6W Blasters (on Pin A7/PA7) directly from the UI.
* **No Coil Whine / High Pitch Noise:** Smart 5ms cool-down delay eliminates parasitic sub-harmonics and inductor whistling.
* **Dynamic Sweep:** Rapidly cycles through core consumer IR frequencies (30, 33, 36, 38, 40, 42, 56 kHz) to blind all TV and AC units simultaneously.

## 🛠 File Structure & Project Manifest

Ensure your `application.fam` looks like this:
```text
App(
    appid="ir_jammer",
    name="IR Jammer",
    apptype=FlipperAppType.EXTERNAL,
    entry_point="ir_jammer_app",
    stack_size=2 * 1024,
    fap_category="Infrared",
    fap_icon="ijamico.png",
)
```

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
