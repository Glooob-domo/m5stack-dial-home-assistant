# Project Structure and Naming

## Root files

- `m5-dial.yaml`: the single ESPHome device file for the dashboard (GitHub package + entity IDs).
- `m5-dial.local.yaml`: compile this clone (`components/` + `src/`) without GitHub. Not used by the ESPHome dashboard.
- `dial.yaml`: firmware package pulled from GitHub by `m5-dial.yaml`.
- `secrets.example.yaml`: public template for credentials.
- `requirements.txt`: pinned ESPHome version for reproducible builds.
- `README.md`: community-facing overview and setup guide.
- `THIRD_PARTY_NOTICES.md`: notes for copied components, fonts, icons, and dependencies.

## Source layout

- `src/main/defaults.yaml`: substitution fallbacks and empty entity lists.
- `src/main/hardware.yaml`: M5Stack Dial hardware drivers and pins.
- `src/main/idle.yaml`: screen dim / return / off timeouts.
- `src/pages/*.yaml`: one LVGL page per feature (`entity_selector.yaml` is shared by climates, media players, covers, garages, switches and scenes).
- `src/assets/fonts/`: local fonts.
- `src/assets/images/`: small embedded image assets.
- `components/`: local ESPHome external components (`dial_lights`, `dial_ha_list`, `dial_climates`, `dial_media_players`, `dial_covers`, `dial_garages`, `dial_switches`, `dial_scenes`, `dial_temperatures`, `dial_carousel`, `dial_i18n`, SendSpin).

## ID conventions

- `page_`: LVGL pages.
- `lbl_`: labels.
- `btn_`: buttons.
- `arc_`: arcs/progress rings.
- `img_`: images.
- `col_`: colors.
- `font_`: fonts.

## What not to publish

- `secrets.yaml`
- `.esphome/`
- `esphome-env/` or `.venv/`
- `.vscode/`
- `mini_code/`
- `__pycache__/`, `*.pyc`, `.DS_Store`
