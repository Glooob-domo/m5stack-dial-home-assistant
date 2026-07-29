# Home Assistant Controller for M5Stack Dial configuration reference

This is the detailed reference for the Home Assistant Controller for M5Stack Dial `dial.yaml` remote package. For an overview and a ready-to-use installation example, return to the [README](../README.md). A normal package installation is configured entirely in your local ESPHome YAML; it does not require editing files inside this repository.

## Device credentials

```yaml
substitutions:
  timezone: Europe/Madrid
  api_encryption_key: !secret api_encryption_key
  wifi_ssid: !secret wifi_ssid
  wifi_password: !secret wifi_password
```

The package defaults `device_name`, `device_friendly_name`, fallback hotspot values and OTA password for validation. Override those values, especially the OTA password, for a real installation. Keep credentials in `secrets.yaml` and never commit them.

## Optional Clock data

```yaml
substitutions:
  timezone: Europe/Madrid
  weather_entity: weather.your_location
  aqi_entity: sensor.your_aqi
```

`weather_entity` and `aqi_entity` are optional: the package defaults them to placeholder entity IDs, so they are not required to compile. If either entity is missing or unavailable, Clock shows incomplete data or `--`. `weather_entity` supplies weather information; `aqi_entity` should be numeric, with the weather entity's legacy `aqi` attribute used only as a fallback. Find entity IDs under **Developer Tools → States**.


## Optional menu features

Unconfigured optional features are hidden from the menu. Timer is hidden while `timer_entity` remains `timer.your_timer`; AC while `climate_entity` remains `climate.your_ac`; and Music while `music_player_entity` remains `media_player.your_player`. Lights is hidden when `dial_lights` has no entries.

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

Select the entity that actually plays the audio. Home Assistant supplies player state, transport actions, volume and available metadata, including `media_title`, `media_artist`, `media_duration` and `media_position`. SendSpin is optional and currently supplies only 100 × 100 album artwork when a compatible SendSpin source is available.

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

Set a timeout to `0s` to disable it. DIM and RETURN are independent. With OFF enabled, the package raises an OFF timeout that is shorter than an enabled DIM or RETURN timeout to the later timeout. DIM and OFF retain the active page; RETURN intentionally navigates to Clock. The first encoder turn, front-button press or touch gesture from DIM or OFF wakes the display and is consumed.

Only an **active** Home Assistant timer blocks RETURN. A paused timer does not. Timer state does not block DIM or OFF. A finished timer wakes the display, opens Timer and triggers its visual blink and buzzer feedback. Remote timer starts do not wake the display.

## Package refresh and validation

`ref: main` follows the version currently published on the repository's `main` branch. `refresh: 0s` makes ESPHome check the remote package on every configuration or build, which is useful while tracking it, but depends on GitHub being reachable and can add download time. It is optional; pin a tag or commit in `ref` for reproducible builds. Recompile after package updates.

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
