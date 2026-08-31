# Project Structure and Naming

## Root files

- `m5-dial.yaml`: the single ESPHome device file (GitHub package + entity IDs).
- `m5-dial.local.yaml`: same wiring, compiling this clone instead of GitHub.
- `dial.yaml`: firmware package pulled from GitHub by `m5-dial.yaml`.
- `secrets.example.yaml`: public template for credentials.
- `requirements.txt`: pinned ESPHome version for reproducible builds.
- `README.md`: community-facing overview and setup guide.
- `THIRD_PARTY_NOTICES.md`: notes for copied components, fonts, icons, and dependencies.

## Source layout

- `src/main/hardware.yaml`: M5Stack Dial hardware drivers and pins.
- `src/main/entities.yaml`: Home Assistant entity bindings.
- `src/pages/*.yaml`: one LVGL page per feature.
- `src/assets/fonts/`: local fonts.
- `src/assets/images/`: small embedded image assets.
- `components/`: local ESPHome external components.

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
