<div align="center">

<img src="src/assets/images/logo.png" width="96" alt="Smart Home Button logo" />

# Smart Home Button

### A circular Home Assistant controller for M5Stack Dial, built with ESPHome and LVGL.

[![ESPHome](https://img.shields.io/badge/ESPHome-2026.7.0-blue?style=flat-square&logo=esphome)](https://esphome.io/)
[![Platform](https://img.shields.io/badge/Platform-ESP32--S3-red?style=flat-square&logo=espressif)](https://www.espressif.com/)
[![Display](https://img.shields.io/badge/Display-GC9A01A%20240x240-purple?style=flat-square)](#hardware)
[![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)](#license)

</div>

Smart Home Button is an independent community project maintained and personalised by [hectorzin](https://github.com/hectorzin). It is not an official project of M5Stack, ESPHome, or Home Assistant.

## What is this?

This firmware turns an M5Stack Dial into a physical Home Assistant controller. The rotary wheel, touch screen and front button make everyday actions—such as checking the room, changing a light, adjusting the climate or controlling music—available without repeatedly opening a phone dashboard.

The interface is built with ESPHome and LVGL, and is organised into pages and reusable components so individual features can be configured or adapted without having to rewrite the entire firmware. This repository is actively maintained as a tailored M5Stack Dial experience, with its own Home Assistant integrations and UI behaviour.

## Features

- Clock, date and weather from Home Assistant.
- Air-quality index (AQI) from a Home Assistant sensor.
- Circular menu navigation with the encoder, touch gestures and front button.
- Configurable Home Assistant lights through `dial_lights`.
- Climate control through a `climate_entity`.
- Media playback, volume and metadata through a `music_player_entity`.
- Home Assistant timer control through a `timer_entity`.
- Menu subtitles based on live Home Assistant states.
- Automatic return to the clock after inactivity.
- Configurable backlight dimming and screen-off behaviour.
- Wake from dimming without leaving the current page.
- Visual and audible notification when a timer finishes.

## Gallery

| | | | |
| --- | --- | --- | --- |
| <img src="docs/images/gallery/clock-weather-page.jpg" alt="Clock and weather screen" width="240"> | <img src="docs/images/gallery/menu-page.jpg" alt="Circular menu" width="240"> | <img src="docs/images/gallery/light-page.jpg" alt="Light control" width="240"> | <img src="docs/images/gallery/ac-page.jpg" alt="Climate control" width="240"> |
| <img src="docs/images/gallery/music-page.jpg" alt="Media-player screen" width="240"> | <img src="docs/images/gallery/timer-page.jpg" alt="Timer screen" width="240"> | | |

## Pages

| Page | Description |
| --- | --- |
| Clock | Shows the time, date, weather and AQI information from Home Assistant. |
| Menu | Circular navigation with live subtitles for configured Home Assistant features. |
| Lights | Controls the Home Assistant light entities declared in `dial_lights`. |
| AC | Shows the state of `climate_entity` and changes its target temperature and power. |
| Music | Controls `music_player_entity`, including playback, volume and available metadata. |
| Timer | Uses `timer_entity` as the source of truth for a Home Assistant countdown timer. |

## Hardware

The firmware targets the M5Stack Dial platform and its ESP32-S3 controller. The configuration uses the Dial's 240 × 240 GC9A01A round display, FT5x06 capacitive touch controller, rotary encoder, front button, PCF8563 RTC, buzzer and display backlight.

M5Dial V1.1 is supported with its battery power-hold configuration: GPIO46 is enabled at boot so the device remains powered when running on battery. Other Dial revisions are not excluded by the configuration, but this battery-specific behaviour is only documented and configured for V1.1.

## Requirements

- A compatible M5Stack Dial.
- Home Assistant.
- ESPHome.
- Wi-Fi access for the Dial.
- Home Assistant entities for each feature you want to enable.

Weather and air quality are used by the main screen. Lights, climate, media player and timer are optional: sections without their configuration disappear from the menu.

## Quick installation

1. In ESPHome, create a new device configuration and add your credentials as secrets.
2. Use the following configuration, replacing the entity IDs with your own Home Assistant entities.
3. Install it on the Dial over USB for the first flash, then use OTA updates as usual.

```yaml
substitutions:
  timezone: Europe/Madrid

  api_encryption_key: !secret api_encryption_key
  wifi_ssid: !secret wifi_ssid
  wifi_password: !secret wifi_password

  weather_entity: weather.estacion_meteorologica
  aqi_entity: sensor.aqi_jardin_pm2_5
  climate_entity: climate.ac_buhardilla
  music_player_entity: media_player.arylic_lp100
  timer_entity: timer.temporizador_dial

  screen_return_timeout: 30s
  screen_dim_timeout: 15s
  screen_off_timeout: 60s
  screen_dim_brightness: "20%"

dial_lights:
  - entity_id: light.sofa
    name: Sofá

  - entity_id: light.mesa
    name: Mesa

  - entity_id: light.luces_jardin
    name: Jardín

packages:
  smart_home_button:
    url: https://github.com/hectorzin/smart-home-button
    ref: main
    files:
      - dial.yaml
    refresh: 0s
```

Create the referenced secrets in ESPHome, for example:

```yaml
api_encryption_key: "replace-with-an-ESPHome-api-key"
wifi_ssid: "your-wifi-network"
wifi_password: "your-wifi-password"
```

For configuration details, optional features, entity requirements, timers and display behaviour, see [the configuration guide](docs/configuration.md).

## License

The project files are available under the MIT License. Third-party components, fonts and icons retain their own licenses; see [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
