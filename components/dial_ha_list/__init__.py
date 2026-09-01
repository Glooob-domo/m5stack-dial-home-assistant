import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.components.text_sensor as text_sensor
from esphome.const import (
    CONF_ACCURACY_DECIMALS,
    CONF_ATTRIBUTE,
    CONF_DISABLED_BY_DEFAULT,
    CONF_ENTITY_ID,
    CONF_FORCE_UPDATE,
    CONF_ID,
    CONF_INTERNAL,
    CONF_NAME,
)
from esphome.core import CORE, ID

CODEOWNERS = []
DEPENDENCIES = ["api", "text_sensor", "sensor"]
AUTO_LOAD = ["text_sensor", "sensor"]

LEGACY_DISABLED = {
    "timer.your_timer",
    "weather.your_location",
}

dial_ha_list_ns = cg.esphome_ns.namespace("dial_ha_list")
DialHaList = dial_ha_list_ns.class_("DialHaList", cg.Component)

ENTITY_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ENTITY_ID): cv.string,
        cv.Required(CONF_NAME): cv.string,
    }
)

# Loaded only via AUTO_LOAD from dial_climates / dial_media_players / dial_covers.
CONFIG_SCHEMA = cv.Schema({})


def entity_enabled(entity):
    if not entity:
        return False
    if entity in LEGACY_DISABLED:
        return False
    if "." in entity and entity.rsplit(".", 1)[1] == "disabled":
        return False
    return True


def list_config_schema(component_id):
    def _schema(value):
        entities = cv.All(cv.ensure_list(ENTITY_SCHEMA), cv.Length(min=0))(value)
        return {
            CONF_ID: cv.declare_id(DialHaList)(component_id),
            "entities": entities,
        }

    return _schema


def _internal_entity_conf(uid, name):
    return {
        CONF_ID: uid,
        CONF_NAME: name,
        CONF_INTERNAL: True,
        CONF_DISABLED_BY_DEFAULT: True,
        CONF_FORCE_UPDATE: False,
    }


async def make_ha_text(uid_prefix, index, suffix, entity_id, attribute=None):
    from esphome.components.homeassistant import setup_home_assistant_entity
    from esphome.components.homeassistant.text_sensor import HomeassistantTextSensor

    uid = ID(f"{uid_prefix}_{index}_{suffix}", True, HomeassistantTextSensor)
    CORE.component_ids.add(uid.id)
    conf = _internal_entity_conf(uid, f"Dial {uid_prefix} {index} {suffix}")
    var = cg.new_Pvariable(uid)
    await text_sensor.register_text_sensor(var, conf)
    await cg.register_component(var, conf)
    ha_conf = {CONF_ENTITY_ID: entity_id, CONF_INTERNAL: True}
    if attribute is not None:
        ha_conf[CONF_ATTRIBUTE] = attribute
    setup_home_assistant_entity(var, ha_conf)
    return var


async def make_ha_number(uid_prefix, index, suffix, entity_id, attribute):
    from esphome.components import sensor as ha_sensor
    from esphome.components.homeassistant import setup_home_assistant_entity
    from esphome.components.homeassistant.sensor import HomeassistantSensor

    uid = ID(f"{uid_prefix}_{index}_{suffix}_num", True, HomeassistantSensor)
    CORE.component_ids.add(uid.id)
    conf = {
        CONF_ID: uid,
        CONF_NAME: f"Dial {uid_prefix} {index} {suffix} num",
        CONF_INTERNAL: True,
        CONF_DISABLED_BY_DEFAULT: True,
        CONF_FORCE_UPDATE: True,
        CONF_ACCURACY_DECIMALS: 0,
    }
    var = cg.new_Pvariable(uid)
    await ha_sensor.register_sensor(var, conf)
    await cg.register_component(var, conf)
    ha_conf = {CONF_ENTITY_ID: entity_id, CONF_INTERNAL: True, CONF_ATTRIBUTE: attribute}
    setup_home_assistant_entity(var, ha_conf)
    return var


async def to_code_list(config, uid_prefix, attributes, numeric_attributes=None):
    cg.add_global(cg.RawStatement('#include <cstdlib>'))
    cg.add_global(cg.RawStatement('#include "esphome/components/dial_lights/dial_entity.h"'))
    cg.add_global(cg.RawStatement('#include "esphome/components/dial_ha_list/dial_ha_list.h"'))
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    numeric_attributes = numeric_attributes or []
    for index, item in enumerate(config["entities"]):
        entity = item[CONF_ENTITY_ID]
        if not entity_enabled(entity):
            continue
        state = await make_ha_text(uid_prefix, index, "state", entity)
        cg.add(var.add_entity(entity, item[CONF_NAME], state))
        for suffix, attribute in attributes:
            sensor = await make_ha_text(uid_prefix, index, suffix, entity, attribute)
            cg.add(var.add_attr(suffix, sensor))
        for suffix, attribute in numeric_attributes:
            sensor = await make_ha_number(uid_prefix, index, suffix, entity, attribute)
            cg.add(var.add_num_attr(suffix, sensor))
    return var


async def to_code(config):
    return
