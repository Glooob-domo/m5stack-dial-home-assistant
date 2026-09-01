from esphome.components.dial_ha_list import list_config_schema, to_code_list

CODEOWNERS = []
DEPENDENCIES = ["dial_ha_list"]
AUTO_LOAD = ["dial_ha_list"]

CONFIG_SCHEMA = list_config_schema("dial_scenes_id")


async def to_code(config):
    await to_code_list(config, "dial_scene", [])
