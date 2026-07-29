# Configuration guide

This guide describes the configuration supplied to the `dial.yaml` remote package. Keep credentials in ESPHome secrets rather than committing them to a configuration file.

## Required main-screen entities

Set these substitutions to Home Assistant entities that exist in your installation:

```yaml
substitutions:
  timezone: Europe/Madrid
  weather_entity: weather.your_location
  aqi_entity: sensor.your_aqi
```

`weather_entity` provides the weather condition. `aqi_entity` must be a numeric sensor. The project can temporarily use the legacy `aqi` attribute from the weather entity when the AQI sensor is unavailable, but a separate sensor is recommended.

Find entity IDs in Home Assistant under **Developer Tools → States**.

## Optional menu sections

The menu is assembled from the configuration. Do not set an option for a feature you do not need: its menu entry will not be shown.

| Configuration | Section | Notes |
| --- | --- | --- |
| `dial_lights` | Lights | Add one or more Home Assistant light entities. |
| `climate_entity` | Climate | Set a Home Assistant climate entity. |
| `music_player_entity` | Music | Set the media player that actually plays audio. |
| `timer_entity` | Timer | Set a Home Assistant `timer` entity. |

### Lights

Each light needs an entity ID and a label:

```yaml
dial_lights:
  - entity_id: light.sofa
    name: Sofá
  - entity_id: light.desk
    name: Escritorio
```

Omit `dial_lights`, or use `dial_lights: []`, to hide Lights.

### Climate

```yaml
substitutions:
  climate_entity: climate.living_room
```

Climate integrations expose different capabilities. The project controls target temperature and power; adapt the configuration or services if your integration requires different fan-mode or swing-mode handling.

### Media player

```yaml
substitutions:
  music_player_entity: media_player.living_room
```

Choose the actual audio player rather than the Dial entity. The most useful integrations expose `volume_level`, `media_title`, `media_artist`, `media_duration`, `media_position`, and `entity_picture`. An unavailable configured player remains configured, but cannot synchronise with the Dial.

### Timer

Create a Timer helper in **Settings → Devices & services → Helpers**, then configure its entity ID:

```yaml
substitutions:
  timer_entity: timer.dial_timer
```

Home Assistant is the source of truth for the timer, so it can also be controlled from automations, dashboards and the mobile app. For YAML-defined timers, `restore: true` is recommended:

```yaml
timer:
  dial_timer:
    name: Dial timer
    duration: "00:05:00"
    restore: true
```

## Display inactivity

These substitutions control the display after inactivity:

```yaml
substitutions:
  screen_dim_timeout: 45s
  screen_return_timeout: 5min
  screen_off_timeout: 30min
  screen_dim_brightness: "20%"
```

| Setting | Default | Effect |
| --- | --- | --- |
| `screen_dim_timeout` | `45s` | Reduces the backlight on the current page. |
| `screen_return_timeout` | `5min` | Returns to the clock, retaining dim brightness. |
| `screen_off_timeout` | `30min` | Turns the display backlight off. |
| `screen_dim_brightness` | `"20%"` | Backlight level while dimmed. |

Set a timeout to `0s` to disable that stage. The first interaction from a dimmed screen restores normal brightness; the first interaction from an off screen wakes the clock. In both cases, the next interaction performs the normal action.

An active or paused timer prevents the screen from fully switching off. When a timer finishes, the Dial wakes, opens the Timer page and keeps its finish alert.

## Credentials and device identity

The remote package provides defaults for the device name, fallback hotspot and OTA password, but override them for a real installation. At minimum, provide Wi-Fi credentials and an ESPHome API encryption key through secrets:

```yaml
substitutions:
  api_encryption_key: !secret api_encryption_key
  wifi_ssid: !secret wifi_ssid
  wifi_password: !secret wifi_password
  device_name: m5stack-dial
  device_friendly_name: M5Stack Dial
  ota_password: !secret ota_password
```

Never publish `secrets.yaml` or a configuration containing real passwords, API keys, or entity names you consider private.

## Build and update

Validate the complete configuration in ESPHome before installing it. Use USB for an initial flash if the device is not yet connected to Wi-Fi; subsequent updates can use ESPHome OTA.
