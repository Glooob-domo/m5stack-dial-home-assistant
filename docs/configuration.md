# Configuration Guide

## 1. Secrets

Copy the example secrets file:

```bash
cp secrets.example.yaml secrets.yaml
```

Fill in your Wi-Fi and ESPHome credentials. `secrets.yaml` is intentionally ignored by Git. The example API key is a public dummy value for config checks only; generate your own before flashing.

## 2. Home Assistant Entities

Edit `src/main/entities.yaml` and replace the placeholders:

```yaml
substitutions:
  weather_entity: weather.your_location
  aqi_entity: sensor.jardin_aqi
```

These two are required for the clock page.

### Optional menu pages

The following are optional. **Do not write a substitution** unless you want that menu item. Home is always shown. Add real entity IDs in `dial.yaml` (or your local substitutions block):

| Option | Menu item | Enable by adding |
|---|---|---|
| `timer_entity` | Timer | Your Home Assistant timer entity ID |
| `climate_entity` | AC | Your climate entity ID |
| `music_player_entity` | Music | Your media player entity ID |
| `dial_lights` in `dial.yaml` | Lights | One or more light entries |

Find entity IDs in Home Assistant under Developer Tools -> States.

`weather_entity` provides weather conditions. Set `aqi_entity` to a numeric Home Assistant sensor for air quality; the legacy `aqi` attribute on the weather entity is used only as a temporary fallback when that sensor is unavailable.

#### Examples

**Home only** — weather and AQI only; no optional substitutions and no `dial_lights`:

```yaml
substitutions:
  weather_entity: weather.estacion_meteorologica
  aqi_entity: sensor.aqi_jardin_pm2_5
```

Do not declare `dial_lights` in `dial.yaml` (the project default is an empty list).

**Music only** — add to `dial.yaml`:

```yaml
substitutions:
  music_player_entity: media_player.arylic_lp100
```

**Lights only**:

```yaml
dial_lights:
  - entity_id: light.sofa
    name: Sofá
```

**No lights** — omit `dial_lights` or set an explicit empty list:

```yaml
dial_lights: []
```

A configured entity still appears in the menu even when Home Assistant reports it as `unavailable`.

## 3. Timer Helper

The Timer page controls a Home Assistant `timer` entity; Home Assistant remains the source of truth for its state and remaining time. Create a Timer helper in **Settings > Devices & services > Helpers**, then set its entity ID as `timer_entity`.

For a YAML-defined helper, use for example:

```yaml
timer:
  temporizador_dial:
    name: Temporizador Dial
    duration: "00:05:00"
    restore: true
```

`restore: true` is recommended so active and paused timers are restored after Home Assistant restarts. Closing the Timer page on the Dial does not pause or cancel the timer; it can also be controlled from the mobile app, dashboards, and automations.

## 4. Music Player Selection

Choose the entity that actually plays audio, not the Dial entity itself. A good target usually exposes:

- `state`: `playing` or `paused`
- `volume_level`
- `media_title`
- `media_artist`
- `media_duration`
- `media_position`
- `entity_picture`

If the entity is `unavailable`, Home Assistant will disable controls and the Dial cannot sync it.

## 5. Timezone

The default timezone is `Europe/Madrid`. Change the `timezone` substitution in `dial.yaml` if you use another timezone.
