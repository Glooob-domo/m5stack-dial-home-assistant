<div align="center">

<img src="src/assets/images/logo.png" width="96" alt="M5Stack Dial Smart Button logo" />

# Smart Home Button

### A Circular HMI for Home Assistant, built with M5Stack Dial, ESPHome, and LVGL.

[![ESPHome](https://img.shields.io/badge/ESPHome-tested%202026.3.3-blue?style=flat-square&logo=esphome)](https://esphome.io/)
[![Platform](https://img.shields.io/badge/Platform-ESP32--S3-red?style=flat-square&logo=espressif)](https://www.espressif.com/)
[![Display](https://img.shields.io/badge/Display-GC9A01A%20240x240-purple?style=flat-square)](#hardware)
[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)](#license)

</div>

## What is this?

This is my M5Stack Dial firmware for controlling a Home Assistant setup from a small round desktop device.

I built it because some smart home actions feel better with a real knob and a small screen. Turning a dial to change brightness, tapping a button for playback, or checking the room status at a glance is faster than opening a phone dashboard every time.

The project is based on ESPHome and LVGL. Most of the UI is split into separate page files, so it is easier to change one feature without touching the rest of the firmware.

## What it can do

- Show time, date, and weather data from Home Assistant.
- Navigate with the rotary encoder, touch gestures, and the front button.
- Control the built-in LED ring with brightness, color, and preset color flows.
- Adjust an air conditioner entity from Home Assistant.
- Show a music page with playback buttons, volume, progress, and album art.
- Control a Home Assistant countdown timer from the Dial.
- Keep the device alive on battery power on M5Dial V1.1 by enabling the power-hold pin.

## Gallery

| | | | |
| --- | --- | --- | --- |
| <img src="docs/images/gallery/music-page.jpg" alt="Music page" width="240"> | <img src="docs/images/gallery/clock-weather-page.jpg" alt="Clock and weather page" width="240"> | <img src="docs/images/gallery/ac-page.jpg" alt="AC page" width="240"> | <img src="docs/images/gallery/timer-page.jpg" alt="Timer page" width="240"> |
| <img src="docs/images/gallery/ac-power-page.jpg" alt="AC power control page" width="240"> | <img src="docs/images/gallery/light-page.jpg" alt="Light page" width="240"> | <img src="docs/images/gallery/menu-page.jpg" alt="Menu page" width="240"> | |

## Pages

| Page | What it does |
| --- | --- |
| Clock | Time, date, weather, humidity, AQI, pressure, and wind speed |
| Menu | Circular page navigation for the small round screen |
| Light | LED ring brightness, color, and effects |
| AC | Target temperature and power control; fan/swing UI can be mapped to your own climate services |
| Music | SendSpin media state, cover art, progress, volume, and transport controls |
| Timer | Home Assistant countdown timer with rotary adjustment |

## Hardware

The firmware is written for M5Stack Dial V1.1.

Main parts used by the project:

- ESP32-S3 controller
- 240 x 240 GC9A01A round display
- FT5x06 capacitive touch
- PCF8563 RTC
- RC522 NFC module on I2C
- Rotary encoder and front button
- SK6812 RGB LED ring
- Buzzer and display backlight
- USB-C for flashing and power

## Project structure

```text
m5dial-smart-button/
|-- dial.yaml                    # ESPHome entry point
|-- secrets.example.yaml         # Copy this to secrets.yaml
|-- requirements.txt             # ESPHome version used for this project
|-- THIRD_PARTY_NOTICES.md        # Third-party code/font/icon notes
|-- src/
|   |-- main/
|   |   |-- hardware.yaml        # M5Dial pins, display, touch, RTC, power hold
|   |   `-- entities.yaml        # Home Assistant entity IDs
|   |-- pages/                   # One LVGL page per feature
|   `-- assets/                  # Fonts and small image assets
|-- components/                  # Local ESPHome components, including SendSpin
|-- hardware/                    # 3D-printable enclosure files
`-- docs/                        # Setup notes and release checklist
```

## Setup

### 1. Install ESPHome

I tested this project with ESPHome `2026.3.3`. The first build may need internet access because ESPHome downloads Google Fonts and build libraries.

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

### 2. Create your secrets file

```bash
cp secrets.example.yaml secrets.yaml
```

Then edit `secrets.yaml`:

```yaml
wifi_ssid: "YOUR_WIFI_SSID"
wifi_password: "YOUR_WIFI_PASSWORD"

ap_ssid: "Dial Fallback Hotspot"
ap_password: "YOUR_FALLBACK_AP_PASSWORD"

api_encryption_key: "YOUR_API_ENCRYPTION_KEY"
ota_password: "YOUR_OTA_PASSWORD"
```

`secrets.yaml` is ignored by Git. Do not publish it.

The API key in `secrets.example.yaml` is only a public dummy value so `esphome config` can run after copying the file. Generate your own key before flashing a real device.

### 3. Set your Home Assistant entities

Edit `src/main/entities.yaml`:

```yaml
substitutions:
  weather_entity: weather.your_location
  aqi_entity: sensor.jardin_aqi
  climate_entity: climate.your_ac
  music_player_entity: media_player.your_player
  timer_entity: timer.temporizador_dial
```

You can find these entity IDs in Home Assistant under **Developer Tools -> States**. `weather_entity` supplies weather conditions, while `aqi_entity` must be a numeric Home Assistant sensor. The legacy `aqi` attribute on `weather_entity` is used only as a temporary fallback when the AQI sensor is unavailable.

For the music page, choose the media player that actually plays audio. It should report useful attributes such as `volume_level`, `media_title`, `media_duration`, and `media_position`. If the entity is `unavailable`, the Dial cannot sync it.

For the timer page, create a Home Assistant Timer helper and use its entity ID for `timer_entity`. For example, a YAML-defined helper can be configured as:

```yaml
timer:
  temporizador_dial:
    name: Temporizador Dial
    duration: "00:05:00"
    restore: true
```

Home Assistant is the timer source of truth: the Dial can be closed without stopping the countdown, and the same timer can also be controlled from dashboards, mobile devices, and automations. `restore: true` is recommended so active and paused timers survive Home Assistant restarts.

### 4. Check, build, and flash

```bash
esphome config dial.yaml
esphome compile dial.yaml
esphome upload dial.yaml --device /dev/cu.usbmodemXXXX
```

For later OTA updates:

```bash
esphome upload dial.yaml
```

## Notes about the music page

The music page uses the local `sendspin` component for media state, transport commands, and album art. The Home Assistant media player entity is also read for status, volume, duration, and progress when those attributes are available.

Album art is intentionally kept small because the M5Dial does not have PSRAM. If you increase the image size or LVGL buffer too much, the device may reboot when artwork is received.

If volume control does not work, first check the Home Assistant media player entity. Some players expose playback state but do not expose writable volume control. If you want a HA-only music page, replace the SendSpin command scripts with standard `media_player` services.

## Notes about AC controls

Climate entities are not all the same. Temperature and power are wired to Home Assistant services in this project. Fan speed and swing are shown as UI controls, but you may need to map them to `climate.set_fan_mode`, `climate.set_swing_mode`, or your own scripts depending on your air conditioner integration.

## Notes about battery power

M5Dial V1.1 needs the hold pin to stay enabled when running from battery. This project turns on GPIO46 during boot in `dial.yaml` and defines the `power_hold` switch in `src/main/hardware.yaml`.

## Common issues

- **Device is unavailable in Home Assistant**: make sure the `api` section is enabled and port `6053` is reachable.
- **Wrong weather values**: change `weather_entity` in `src/main/entities.yaml`.
- **AQI shows `--`**: set `aqi_entity` in `src/main/entities.yaml` to a numeric Home Assistant sensor.
- **Wrong AC entity**: change `climate_entity` in `src/main/entities.yaml`.
- **Music page shows unavailable**: choose the real player entity, not the Dial entity itself.
- **Timer page shows unavailable**: create the Timer helper in Home Assistant and check `timer_entity` in `src/main/entities.yaml`.
- **Album art causes reboot**: keep the artwork small and do not increase LVGL memory use too much.
- **First build cannot download fonts**: connect to the internet once so ESPHome can fetch Google Fonts, or replace `gfonts://` fonts with local font files.
- **Battery does not keep the device on**: check the V1.1 power-hold configuration.

## What you may want to change

Most people will need to edit only these files:

- `secrets.yaml` for Wi-Fi and ESPHome credentials.
- `src/main/entities.yaml` for Home Assistant entity IDs.
- `src/pages/main.yaml` if you want a different timezone or weather layout.
- `src/pages/music.yaml` if you want to customize the music UI.

## License

MIT License for the original project files. See `LICENSE`.

Third-party code, fonts, icons, and build dependencies keep their own licenses. See `THIRD_PARTY_NOTICES.md`.

## Demo Video

Watch the demo on YouTube: [Smart Home Button demo](https://www.youtube.com/watch?v=51bXRBuSLpM)
