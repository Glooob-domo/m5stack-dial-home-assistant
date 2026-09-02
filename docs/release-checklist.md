# Open Source Release Checklist

Before publishing to GitHub, Hackster, or a ZIP download:

- [ ] Confirm `secrets.yaml` is not included.
- [ ] Confirm `.esphome/`, `esphome-env/`, `.venv/`, `.vscode/`, and `mini_code/` are not included.
- [ ] Run a sensitive scan for Wi-Fi names, passwords, API keys, local IPs, and local paths.
- [ ] Check that public Git commit author name/email are OK to publish.
- [ ] Replace local Home Assistant entities in `m5-dial.yaml` with placeholders or examples.
- [ ] Run `esphome config m5-dial.local.yaml` using placeholder secrets.
- [ ] Confirm README and `docs/configuration.md` match current UI behaviour (encoder, clock, garage, music placeholder, Rooms page).
- [ ] Add photos, demo GIFs, and setup notes for the community post.
- [ ] Keep third-party license notices for copied components, fonts, and icons.
- [ ] Publish from a fresh Git repository or a clean export folder, not from old history that may contain secrets.
