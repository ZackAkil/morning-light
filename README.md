# Morning Light

Gradual sunrise/sunset wake-up light controller for the **Arduino Yún** driving an addressable WS2813/NeoPixel LED strip, configured over the local network via a lightweight built-in web UI.

## Features

- **Offline-resilient clock:** Syncs with the Yún's OpenWrt Linux clock every 60s and tracks time locally on the AVR in between.
- **Gradual sunrise & sunset:** Configurable On/Off times, fade-in/fade-out durations (`0–180` min), and start/end hues + brightness (`0–100%`), persisted in `EEPROM`.
- **Instant manual override & live tester:** Toggle the strip immediately or preview any hue and brightness in real time from the browser.

## Hardware & Wiring

- **Board:** Arduino Yún (`arduino:avr:yun`)
- **LED Strip:** 21x WS2813 (5V dual-signal addressable RGB strip)
- **Power:** Regulated 5V 3A supply

| Strip / Supply | Arduino Yún |
| :--- | :--- |
| 5V Supply `+` & Strip `+5V` | `VIN` *(regulated 5V only)* |
| 5V Supply `-` & Strip `GND` | `GND` |
| Strip `DI` *(input end)* | Digital Pin `6` |
| Strip `BI` *(input end)* | `GND` |

## Dependencies

Install via Arduino IDE Library Manager or `arduino-cli`:

- `Bridge` (`>= 1.7.0`)
- `Adafruit NeoPixel` (`>= 1.15.0`)

## Usage

1. Connect the Arduino Yún to your 2.4 GHz Wi-Fi network.
2. Open `sketch_oct30a/sketch_oct30a.ino` in Arduino IDE, select **Arduino Yún**, and upload.
3. Wait for the built-in LED to blink twice, then open:
   ```text
   http://<yun-ip>/arduino/home
   ```

## Project Structure

```text
sketch_oct30a/
├── sketch_oct30a.ino          # Entry point (setup & loop)
└── src/
    ├── core/                  # Shared types, string codec, EEPROM schedule logic
    ├── hardware/              # Linux clock sync & NeoPixel fade engine
    └── web/                   # BridgeServer HTTP router & PROGMEM web UI
        └── assets/            # Static CSS, HTML markup, and client JS
```

## License

Licensed under the [Apache License 2.0](LICENSE).
