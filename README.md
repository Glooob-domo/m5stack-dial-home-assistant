# Smart Home Button for M5Stack Dial

Smart Home Button is an ESPHome interface for the M5Stack Dial that integrates with Home Assistant. It turns the Dial's round display, rotary encoder, touch screen and button into a compact control surface for a smart home.

It currently includes:

- Clock and main status screen.
- Circular navigation menu.
- Light control.
- Climate control.
- Media-player control.
- Home Assistant timer control.
- Weather and air-quality information.
- Inactivity dimming and screen-off behaviour.
- Automatic return to the clock.
- Menu subtitles that reflect live Home Assistant states.

This is an independent community project; it is not an official project of M5Stack, ESPHome, or Home Assistant.

## Screens

| | | | |
| --- | --- | --- | --- |
| <img src="docs/images/gallery/clock-weather-page.jpg" alt="Clock and weather screen" width="240"> | <img src="docs/images/gallery/menu-page.jpg" alt="Circular menu" width="240"> | <img src="docs/images/gallery/light-page.jpg" alt="Light control" width="240"> | <img src="docs/images/gallery/ac-page.jpg" alt="Climate control" width="240"> |
| <img src="docs/images/gallery/music-page.jpg" alt="Media-player screen" width="240"> | <img src="docs/images/gallery/timer-page.jpg" alt="Timer screen" width="240"> | | |

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
