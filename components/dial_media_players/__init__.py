from esphome.components.dial_ha_list import list_config_schema, to_code_list

CODEOWNERS = []
DEPENDENCIES = ["dial_ha_list"]
AUTO_LOAD = ["dial_ha_list"]

CONFIG_SCHEMA = list_config_schema("dial_media_players_id")

MEDIA_ATTRS = [
    ("title", "media_title"),
    ("artist", "media_artist"),
    ("volume", "volume_level"),
    ("duration", "media_duration"),
    ("position", "media_position"),
]


async def to_code(config):
    await to_code_list(config, "dial_media", MEDIA_ATTRS)
