# Contributing

Thanks for improving Home Assistant Controller for M5Stack Dial.

## Before opening a pull request

- Keep credentials out of the repository.
- Run `esphome config m5-dial.local.yaml` before submitting changes (ESPHome 2026.7 or later).
- Keep page files focused: one feature page per YAML file in `src/pages/`.
- Avoid committing generated build output, virtual environments, caches, or local IDE settings.
- Document any new Home Assistant entity dependency in `docs/configuration.md`.

## Code style

- Use lowercase snake_case filenames.
- Use clear LVGL ID prefixes: `page_`, `lbl_`, `btn_`, `arc_`, `img_`.
- Prefer `m5-dial.yaml` for Home Assistant entity IDs (one category per page).
- Keep image assets small enough for ESP32-S3 without PSRAM.
