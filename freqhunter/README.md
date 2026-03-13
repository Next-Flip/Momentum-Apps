# FreqHunter 🎯
### Sub-GHz Scanner, Spectrum Analyzer, Decoder & Sender for Flipper Zero

FreqHunter is an all-in-one Sub-GHz toolkit. It combines a live RSSI waveform
scanner, a spectrum monitor, a ProtoView-style pulse decoder, and a `.sub` file
transmitter — all in a single app with an animated home menu.

---

## Features

| Feature | Detail |
|---|---|
| **Scanner** | Live 110-column scrolling RSSI waveform across 7 ISM frequencies |
| **Spectrum** | Heartbeat-style RSSI monitor with peak hold on the selected frequency |
| **Decoder** | Async pulse capture with ProtoView-style waveform display and protocol ID |
| **Send .sub** | Browse SD card for `.sub` files and transmit them via the CC1101 |
| **Settings** | Sound toggle, modulation preset, frequency, auto-hop on/off |
| **CSV Logging** | `timestamp_ms, freq_hz, freq_label, rssi_dbm` appended to SD card |
| **Auto-hop** | Cycle through all frequencies automatically at configurable speed |
| **Modulations** | AM650, AM270, FM238, FM476 (CC1101 custom register presets) |

---

## Frequencies

| Label | Hz |
|---|---|
| 315 | 315 000 000 |
| 433 | 433 920 000 |
| 434 | 434 420 000 |
| 868 | 868 000 000 |
| 868.35 | 868 350 000 |
| 915 | 915 000 000 |
| 928 | 928 000 000 |

---

## Pages & Controls

### Home
| Button | Action |
|---|---|
| **Up / Down** | Navigate menu (scrolls if needed) |
| **OK** | Open selected page |
| **Back (hold 2s)** | Exit app |

### Scanner
| Button | Action |
|---|---|
| **Up / Down** | Change frequency |
| **Left / Right** | Adjust detection threshold (5 dBm steps) |
| **OK** | Start / stop CSV logging |
| **OK (hold)** | Toggle auto-hop |
| **Right (hold 3s)** | Jump to Decoder |

### Spectrum
| Button | Action |
|---|---|
| **OK** | Reset peak hold |
| **Back** | Return to Home |

### Decoder
| Button | Action |
|---|---|
| **Up / Down** | Change frequency, restart capture |
| **OK** | Clear capture and listen again |
| **Left (hold 3s)** | Jump to Scanner |

### Send .sub
| Button | Action |
|---|---|
| **OK** | Browse SD card for `.sub` file |
| **OK** (after load) | Transmit the loaded file |
| **OK** (after done) | Transmit again |
| **Back** | Return to Home |

### Settings
| Button | Action |
|---|---|
| **Up / Down** | Navigate settings |
| **OK / Left / Right** | Change value |

---

## Screen Layout (Scanner)

```
Scanner                     [LOG][HOP]
433.920 MHz  433              7/7
RSSI: -82          [AM650]
T:-85 Pk:-78            Det:3
┌──────────────────────────────────┐
│  ▄  ▄▄    ▄  ▄▄▄   ▄            │  ← waveform spikes
│- - - - - - - - - - - - - - - - -│  ← threshold
└──────────────────────────────────┘
Log:12                      OK:Log
```

---

## Log File

Saved to `/ext/apps_data/freqhunter/log.csv`:

```
timestamp_ms,freq_hz,freq_label,rssi_dbm
12345,433920000,433,-78
```

---

## Build

Requires [ufbt](https://github.com/flipperdevices/flipperzero-ufbt):

```sh
ufbt build
ufbt launch   # build + deploy over USB
```

---

## Author

**Smoodiehacking** — [github.com/OscarLauen/FreqHunter](https://github.com/OscarLauen/FreqHunter)

---

## Log File

```
/ext/apps_data/freqhunter/log.csv
```

CSV format:
```
timestamp_ms,frequency_hz,rssi_dbm
12345,433920000,-72.5
12427,433920000,-68.1
```

Open in Excel, Python/pandas, or any CSV viewer.

---

## Build & Install

### Prerequisites
```bash
brew install pipx
pipx install ufbt
pipx ensurepath
```

### Build
```bash
cd FreqHunter
ufbt
```
Output: `dist/freqhunter.fap`

### Deploy to Flipper (USB)
Close qFlipper first, then:
```bash
ufbt launch
```

### Manual install
Copy `dist/freqhunter.fap` to your Flipper SD card:
```
/ext/apps/Tools/freqhunter.fap
```
Find it under **Applications → Tools → FreqHunter**.

---

## Frequency Hopping

By default FreqHunter stays on one frequency so you can focus.
To enable automatic sweeping, uncomment this block in `freqhunter.c`:

```c
app->freq_index = (app->freq_index + 1) % FREQ_COUNT;
radio_start_rx(app);
```

---

## Adding Custom Frequencies

Edit `FREQ_LIST` in `freqhunter.c`:

```c
static const uint32_t FREQ_LIST[] = {
    300000000,   /* 300.0 MHz */
    433920000,   /* 433.92 MHz */
    // add yours here in Hz
};
```

Valid range: **300 000 000 – 928 000 000 Hz**

---

## Author

Made by oscarlauenbach
