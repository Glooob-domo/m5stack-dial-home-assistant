# Configuration reference

This is the detailed reference for the `dial.yaml` remote package. For an overview and a ready-to-use installation example, return to the [README](../README.md). A normal package installation is configured entirely in your local ESPHome YAML; it does not require editing files inside this repository.

## Required substitutions

```yaml
substitutions:
  timezone: Europe/Madrid
  api_encryption_key: !secret api_encryption_key
  wifi_ssid: !secret wifi_ssid
  wifi_password: !secret wifi_password
  weather_entity: weather.your_location
  aqi_entity: sensor.your_aqi
```

`weather_entity` supplies the main weather information. `aqi_entity` must be a numeric Home Assistant sensor; the weather entity's legacy `aqi` attribute is only a fallback. Find entity IDs under **Developer Tools → States**.

The package defaults `device_name`, `device_friendly_name`, fallback hotspot values and OTA password for validation. Override those values, especially the OTA password, for a real installation. Keep credentials in `secrets.yaml` and never commit them.

## Optional menu features

Unconfigured optional features are hidden from the menu. The package uses placeholder defaults for climate, music and timer, so add only the substitutions you want.

| Field | Enables | Example |
| --- | --- | --- |
| `dial_lights` | Lights | A list of Home Assistant light entities. |
| `climate_entity` | AC | `climate.living_room` |
| `music_player_entity` | Music | `media_player.living_room` |
| `timer_entity` | Timer | `timer.dial_timer` |

### Lights

```yaml
dial_lights:
  - entity_id: light.sofa
    name: Sofá
  - entity_id: light.desk
    name: Escritorio
```

Each entry needs an `entity_id` and a display `name`. Omit the key, or set `dial_lights: []`, to disable Lights and hide its menu entry.

### Climate

```yaml
substitutions:
  climate_entity: climate.living_room
```

The page supports target temperature and uses modes advertised by the entity. HVAC mode, fan mode and swing-related capabilities vary between Home Assistant integrations, so only controls supported by the entity should be expected.

### Music

```yaml
substitutions:
  music_player_entity: media_player.living_room
```

Select the entity that actually plays the audio. Useful attributes include `volume_level`, `media_title`, `media_artist`, `media_duration` and `media_position`. The package also includes active SendSpin components for synchronised playback data and 100 × 100 album art when a SendSpin source is present.

### Timer

```yaml
substitutions:
  timer_entity: timer.dial_timer
```

Create the referenced Home Assistant Timer helper. Home Assistant remains the source of truth, so dashboards and automations can control it too. For YAML-defined timers, `restore: true` is optional but can be useful after a Home Assistant restart.

```yaml
timer:
  dial_timer:
    name: Dial timer
    duration: "00:05:00"
    restore: true
```

## Screen-management reference

```yaml
substitutions:
  screen_dim_timeout: 45s
  screen_return_timeout: 5min
  screen_off_timeout: 30min
  screen_dim_brightness: "20%"
```

| Setting | Default | Behaviour |
| --- | --- | --- |
| `screen_dim_timeout` | `45s` | Dims the current page's backlight. |
| `screen_return_timeout` | `5min` | Returns to Clock. |
| `screen_off_timeout` | `30min` | Turns off only the display backlight. |
| `screen_dim_brightness` | `"20%"` | Backlight level during DIM. |

Set a timeout to `0s` to disable it. DIM and RETURN are independent. With OFF enabled, the package raises an OFF timeout that is shorter than an enabled DIM or RETURN timeout to the later timeout. The active page is retained through DIM and OFF; the first interaction wakes the display and is consumed.

Only an **active** Home Assistant timer blocks RETURN. A paused timer does not. Timer state does not block DIM or OFF. A finished timer wakes the display, opens Timer and triggers its visual blink and buzzer feedback. Remote timer starts do not wake the display.

## Package refresh and validation

`refresh: 0s` makes ESPHome check the remote package on each build, which is useful while following project changes. Recompile after package updates. For production, choose a refresh period appropriate to your update policy.

Validate before flashing:

```bash
esphome config your-dial.yaml
esphome compile your-dial.yaml
```

Use USB for the initial installation if the device is not on Wi-Fi; later updates can use ESPHome OTA.

## Technical troubleshooting

- **Invalid API encryption key:** generate a valid ESPHome API key and put it in `secrets.yaml`.
- **Entity not found or unavailable:** check its exact ID and availability in Home Assistant; a configured unavailable feature stays in the menu but cannot synchronise.
- **Lights missing:** ensure `dial_lights` has at least one entry and is not `[]`.
- **AQI empty:** use a numeric sensor, not a textual state.
- **Fonts or glyphs fail during build:** allow the initial build to download Google Fonts and dependencies; use the version pinned in `requirements.txt`.
- **Compilation error after an update:** validate the complete local YAML and refresh the package before retrying.

## Development-only customisation

Clone the repository when you need to change the firmware itself. `src/pages/` contains the LVGL pages, while `src/main/` contains hardware, default entities, idle logic and light sensors. Install `requirements.txt`, copy `secrets.example.yaml` to a local `secrets.yaml`, and run `esphome config dial.yaml` before compiling. These internal files are not part of the normal remote-package workflow.
