from esphome.components.dial_ha_list import list_config_schema, to_code_list

CODEOWNERS = []
DEPENDENCIES = ["dial_ha_list"]
AUTO_LOAD = ["dial_ha_list"]

CONFIG_SCHEMA = list_config_schema("dial_climates_id", "dial_climates", {"climate"})

CLIMATE_ATTRS = [
    ("temperature", "temperature"),
    ("current_temperature", "current_temperature"),
    ("humidity", "current_humidity"),
    ("hvac_modes", "hvac_modes"),
    ("fan_mode", "fan_mode"),
    ("fan_modes", "fan_modes"),
]


async def to_code(config):
    await to_code_list(config, "dial_climate", CLIMATE_ATTRS)
