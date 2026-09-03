# Home Assistant Controller for M5Stack Dial — configuration reference

Maintained by **[Glooob Domo](https://github.com/Glooob-domo)**, based on [hectorzin](https://github.com/hectorzin)'s Home Assistant Controller fork and [Jason Wen](https://github.com/Jasionf)'s original Smart Home Button project. For an overview and a ready-to-use installation example, return to the [README](../README.md).

**Languages:** English (this page) · [Français](configuration.fr.md)

A normal install uses one file in ESPHome: [`m5-dial.EN.yaml`](../m5-dial.EN.yaml) (English comments) or [`m5-dial.FR.yaml`](../m5-dial.FR.yaml) (French comments) — same fields either way. It pulls the firmware from GitHub and holds your entity IDs. You do not copy `src/` or `components/` into ESPHome.

## Device credentials

```yaml
substitutions:
  timezone: Europe/Paris
  api_encryption_key: !secret api_encryption_key
  wifi_ssid: !secret wifi_ssid
  wifi_password: !secret wifi_password
```

The package defaults `device_name`, `device_friendly_name`, fallback hotspot values and OTA password for validation. Override those values, especially the OTA password, for a real installation. Keep credentials in `secrets.yaml` and never commit them.

## Language

```yaml
substitutions:
  ui_language: fr
```

`ui_language` picks the labels on Clock, the menu and the other pages. Supported values: `en`, `fr`, `es`, `de`, `it`. Changing it requires a recompile. The default is `en` if the key is omitted.

Non-English locales also switch the **date order** on Clock to day/month (`Mar  02/09`). English keeps month/day (`Mon  09/02`).

## Clock display

The clock page is always available. Besides time, weather and optional AQI, it shows a circular **seconds** progress ring that updates every 5 seconds.

```yaml
substitutions:
  timezone: Europe/Paris
  ui_language: fr
  weather_entity: weather.maison
  aqi_entity: sensor.aqi_salon
```

Leave `weather.disabled` or `sensor.disabled` in `m5-dial.FR.yaml` / `m5-dial.EN.yaml` to keep `--` on those fields. If an entity is missing or unavailable, Clock also shows `--`. `weather_entity` supplies weather information; `aqi_entity` should be numeric, with the weather entity's legacy `aqi` attribute used only as a fallback. Find entity IDs under **Developer Tools → States**.

## Optional menu features

Unconfigured optional features are hidden from the menu. Fill the matching list to show a page. **Omit** a list key entirely to hide that page — you do not need to add empty `dial_*: []` entries in `m5-dial.FR.yaml` / `m5-dial.EN.yaml`. Clock weather/AQI stay on the clock page and show `--` when left as `*.disabled`. Alarm needs no field at all — it is always in the menu, set entirely on the Dial.

| Field | Enables | Example |
| --- | --- | --- |
| `dial_lights` | Lights | A list of Home Assistant light entities. |
| `dial_climates` | AC | A list of climate entities. |
| `dial_media_players` | Music | A list of media players. |
| `dial_covers` | Covers | A list of cover / shutter entities. |
| `dial_garages` | Garage | A list of garage / gate cover entities. |
| `dial_switches` | Outlets | A list of `switch` or `input_boolean` entities. |
| `dial_scenes` | Scenes | A list of `scene` or `script` entities. |
| `dial_temperatures` | Rooms | A list of temperature sensors (or climate entities). |

### Lights

```yaml
dial_lights:
  - entity_id: light.living_room
    name: Living room
  - entity_id: light.desk
    name: Desk
```

Each entry needs an `entity_id` and a display `name`. Omit `dial_lights` to hide Lights from the menu. One light opens the control page directly; two or more open a selector first.

### Climate

```yaml
dial_climates:
  - entity_id: climate.living_room
    name: Living room
  - entity_id: climate.bedroom
    name: Bedroom
```

Same rule as lights: one climate opens the page directly, several open a selector.

The page supports target temperature and uses modes advertised by the entity. HVAC mode, fan mode and swing-related capabilities vary between Home Assistant integrations, so only controls supported by the entity should be expected.

### Music

```yaml
dial_media_players:
  - entity_id: media_player.living_room
    name: Living room
```

Select the entity that actually plays the audio. Home Assistant supplies player state, transport actions, volume and available metadata, including `media_title`, `media_artist`, `media_duration` and `media_position`.

Without **SendSpin** (optional — a compatible source like Music Assistant pushing a 100 × 100 JPEG cover to the Dial), the page uses a bigger layout: a large volume dial instead of the cover, and bigger title/artist/transport controls. The first cover SendSpin ever delivers switches the page, for the rest of that boot, to the compact layout (small cover + small volume bars). Playback and metadata work the same either way.

### Covers

```yaml
dial_covers:
  - entity_id: cover.living_room
    name: Living room
  - entity_id: cover.bedroom
    name: Bedroom
```

The encoder sets position in 5% steps. A short press toggles open/close (or stop if the cover is moving). Touch buttons send open, stop and close. With the default `encoder_resolution: "1"`, one mechanical click equals one 5% step.

### Garage

```yaml
dial_garages:
  - entity_id: cover.gate
    name: Gate
```

Same skip-if-one rule as covers. The page shows **open** and **close** touch buttons in the centre and **stop** below. The encoder is ignored. A short press stops the cover while it is opening or closing. There is no position arc or percentage.

### Outlets

```yaml
dial_switches:
  - entity_id: switch.living_room
    name: Living room
```

A short press or tap toggles the switch. `input_boolean.*` helpers are accepted in the same list.

### Scenes

```yaml
dial_scenes:
  - entity_id: scene.movie
    name: Movie
  - entity_id: script.all_off
    name: All off
```

A short press or tap runs `scene.turn_on` or `script.turn_on` according to the entity domain. The control page shows a large **ACTIVATE** button in the centre.

### Rooms

```yaml
dial_temperatures:
  - entity_id: sensor.living_room_temperature
    name: Living room
  - entity_id: sensor.bedroom_temperature
    name: Bedroom
```

Each entry needs an `entity_id` and a display `name`. Use a numeric `sensor.*` (the state is the temperature) or a `climate.*` entity (the page shows `current_temperature`). Omit `dial_temperatures` to hide the menu entry. One room opens the page directly; several rooms stay on that page and the encoder steps through them.

### Alarm

Nothing to configure — Alarm is always in the menu. It is a single daily alarm, entirely local to the Dial: the target time and armed state live on the device (they survive a reboot) and there is no Home Assistant entity involved.

Press once on the Alarm page to edit the hour (it blinks; the encoder adjusts it), press again to move to the minute, then once more to confirm. Tap **ON**/**OFF** to arm or disarm it. Once armed, it fires at that time every day until disarmed. When it fires, the Dial wakes, opens Alarm and beeps every couple of seconds until dismissed (tap the screen or press the button).

## Encoder

```yaml
substitutions:
  encoder_resolution: "1"
```

The Dial encoder produces four quadrature pulses per mechanical click. `encoder_resolution` is how many of those pulses count as one UI step. Allowed values: `1`, `2` or `4`. The default **`1`** is one UI step per mechanical click on every page that uses the wheel (lights, covers, rooms, volume, alarm editing, menu selection, etc.). Use `4` only if you want a more sensitive wheel. Changing it requires a recompile.

### Value step size

On the pages that edit an entity value (Lights, Covers, AC, Music), each encoder step listed above changes the value by a configurable amount:

```yaml
substitutions:
  cover_position_step: "5"       # % per step, 1-100
  light_brightness_step: "1"     # % per step, 1-100
  climate_temperature_step: "2"  # 0.5C units per step: 2 = 1C, 1 = 0.5C
  music_volume_step: "10"        # % per step, 1-100
```

These only apply to Lights, Covers, AC and Music — pages that only navigate (Menu, Rooms) or toggle/activate (Outlets, Scenes, Garage) are unaffected. Changing any of them requires a recompile.

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

Only a **ringing** Alarm blocks RETURN; DIM and OFF are unaffected either way. A firing Alarm wakes the display, opens Alarm and triggers its visual blink and buzzer feedback until dismissed.

## Package refresh and validation

`ref: main` follows the version currently published on the repository's `main` branch. `refresh: 0s` makes ESPHome check the remote package on every configuration or build, which is useful while tracking it, but depends on GitHub being reachable and can add download time. It is optional; pin a tag or commit in `ref` for reproducible builds. Recompile after package updates.

Validate before flashing:

```bash
esphome config your-dial.yaml
esphome compile your-dial.yaml
```

Use USB for the initial installation if the device is not on Wi-Fi; later updates can use ESPHome OTA.

## Common configuration errors

Each `dial_*` list validates its entities **at compile time**, before you ever flash the device:

- **Wrong domain:** an entity under the wrong list (for example `switch.salon` under `dial_lights`) fails `esphome config`/`compile` with an explicit message, e.g. `Entity ID 'switch.salon' is not valid for dial_lights: expected an entity starting with 'light.'`. Move the entry to the matching list or fix the typo in the domain.
- **Duplicate entity:** listing the same `entity_id` twice in one list (a common copy-paste mistake) fails with `Duplicate entity_id '...' in dial_switches: already used for '...'`. Remove the extra entry.

`*.disabled` placeholders are exempt from both checks, so `weather.disabled` and `sensor.disabled` stay valid regardless of list.

## Technical troubleshooting

- **Invalid API encryption key:** generate a valid ESPHome API key and put it in `secrets.yaml`.
- **Entity not found or unavailable:** check its exact ID and availability in Home Assistant; a configured unavailable feature stays in the menu but cannot synchronise.
- **Lights missing:** ensure `dial_lights` has at least one entry and is not `[]`.
- **AQI empty:** use a numeric sensor, not a textual state.
- **Music is unavailable:** use the entity that actually plays audio and exposes its media state.
- **Album art never appears:** enable SendSpin on the music source and point it at the Dial; without SendSpin the Home Assistant logo placeholder remains.
- **Fonts or glyphs fail during build:** allow the initial build to download Google Fonts and dependencies; use the version pinned in `requirements.txt`.
- **Compilation error after an update:** validate the complete local YAML and refresh the package before retrying.

## Development-only customisation

Clone the repository when you need to change the firmware itself. Entity IDs belong in `m5-dial.FR.yaml` / `m5-dial.EN.yaml` (dashboard). To compile this clone without GitHub, use `m5-dial.local.yaml`. `src/pages/` contains the LVGL pages; `src/main/` contains hardware, idle logic and default fallbacks. Install `requirements.txt`, copy `secrets.example.yaml` to a local `secrets.yaml`, and run `esphome config m5-dial.local.yaml` before compiling.
