import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.components.text_sensor as text_sensor
from esphome.const import (
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
DEPENDENCIES = ["api", "text_sensor"]
AUTO_LOAD = ["text_sensor"]

CONF_STATE_SENSOR = "state_sensor"
CONF_MODES_SENSOR = "modes_sensor"
CONF_BRIGHTNESS_SENSOR = "brightness_sensor"
CONF_COLOR_SENSOR = "color_sensor"
CONF_COLOR_MODE_SENSOR = "color_mode_sensor"
CONF_COLOR_TEMP_KELVIN_SENSOR = "color_temp_kelvin_sensor"
CONF_MIN_COLOR_TEMP_KELVIN_SENSOR = "min_color_temp_kelvin_sensor"
CONF_MAX_COLOR_TEMP_KELVIN_SENSOR = "max_color_temp_kelvin_sensor"

LEGACY_DISABLED = {
    "timer.your_timer",
    "weather.your_location",
}

dial_lights_ns = cg.esphome_ns.namespace("dial_lights")
DialLights = dial_lights_ns.class_("DialLights", cg.Component)


def entity_enabled(entity):
    if not entity:
        return False
    if entity in LEGACY_DISABLED:
        return False
    if "." in entity and entity.rsplit(".", 1)[1] == "disabled":
        return False
    return True


def _validate_light_entity(entity):
    if not entity_enabled(entity):
        return entity
    if not entity.startswith("light."):
        raise cv.Invalid(
            f"Entity ID '{entity}' is not valid for dial_lights: "
            f"expected an entity starting with 'light.'"
        )
    return entity


def _reject_duplicate_lights(lights):
    seen = {}
    for light in lights:
        entity = light[CONF_ENTITY_ID]
        if not entity_enabled(entity):
            continue
        if entity in seen:
            raise cv.Invalid(
                f"Duplicate entity_id '{entity}' in dial_lights: "
                f"already used for '{seen[entity]}'"
            )
        seen[entity] = light[CONF_NAME]
    return lights


LIGHT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ENTITY_ID): cv.All(cv.string, _validate_light_entity),
        cv.Required(CONF_NAME): cv.string,
        cv.Optional(CONF_STATE_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_MODES_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_BRIGHTNESS_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_COLOR_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_COLOR_MODE_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_COLOR_TEMP_KELVIN_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_MIN_COLOR_TEMP_KELVIN_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_MAX_COLOR_TEMP_KELVIN_SENSOR): cv.use_id(text_sensor.TextSensor),
    }
)


def _config_schema(value):
    lights = cv.All(
        cv.ensure_list(LIGHT_SCHEMA), cv.Length(min=0), _reject_duplicate_lights
    )(value)
    return {
        CONF_ID: cv.declare_id(DialLights)("dial_lights_id"),
        "lights": lights,
    }


CONFIG_SCHEMA = _config_schema


def _internal_entity_conf(uid, name):
    return {
        CONF_ID: uid,
        CONF_NAME: name,
        CONF_INTERNAL: True,
        CONF_DISABLED_BY_DEFAULT: True,
        CONF_FORCE_UPDATE: False,
    }


async def _make_ha_text(index, suffix, entity_id, attribute=None):
    from esphome.components.homeassistant import setup_home_assistant_entity
    from esphome.components.homeassistant.text_sensor import HomeassistantTextSensor

    uid = ID(f"dial_light_{index}_{suffix}", True, HomeassistantTextSensor)
    CORE.component_ids.add(uid.id)
    conf = _internal_entity_conf(uid, f"Dial light {index} {suffix}")
    var = cg.new_Pvariable(uid)
    await text_sensor.register_text_sensor(var, conf)
    await cg.register_component(var, conf)
    ha_conf = {CONF_ENTITY_ID: entity_id, CONF_INTERNAL: True}
    if attribute is not None:
        ha_conf[CONF_ATTRIBUTE] = attribute
    setup_home_assistant_entity(var, ha_conf)
    return var


async def to_code(config):
    # Loaded with github:// components, so lambdas work without a local includes: path.
    cg.add_global(cg.RawStatement('#include "esphome/components/dial_lights/dial_entity.h"'))
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    for index, light in enumerate(config["lights"]):
        entity = light[CONF_ENTITY_ID]
        if not entity_enabled(entity):
            continue
        name = light[CONF_NAME]
        if CONF_STATE_SENSOR in light:
            state = await cg.get_variable(light[CONF_STATE_SENSOR])
        else:
            state = await _make_ha_text(index, "state", entity)
        if CONF_MODES_SENSOR in light:
            modes = await cg.get_variable(light[CONF_MODES_SENSOR])
        else:
            modes = await _make_ha_text(index, "modes", entity, "supported_color_modes")
        if CONF_BRIGHTNESS_SENSOR in light:
            brightness = await cg.get_variable(light[CONF_BRIGHTNESS_SENSOR])
        else:
            brightness = await _make_ha_text(index, "brightness", entity, "brightness")
        if CONF_COLOR_SENSOR in light:
            color = await cg.get_variable(light[CONF_COLOR_SENSOR])
        else:
            color = await _make_ha_text(index, "rgb", entity, "rgb_color")
        if CONF_COLOR_MODE_SENSOR in light:
            color_mode = await cg.get_variable(light[CONF_COLOR_MODE_SENSOR])
        else:
            color_mode = await _make_ha_text(index, "color_mode", entity, "color_mode")
        if CONF_COLOR_TEMP_KELVIN_SENSOR in light:
            color_temp_kelvin = await cg.get_variable(light[CONF_COLOR_TEMP_KELVIN_SENSOR])
        else:
            color_temp_kelvin = await _make_ha_text(index, "kelvin", entity, "color_temp_kelvin")
        if CONF_MIN_COLOR_TEMP_KELVIN_SENSOR in light:
            min_color_temp_kelvin = await cg.get_variable(light[CONF_MIN_COLOR_TEMP_KELVIN_SENSOR])
        else:
            min_color_temp_kelvin = await _make_ha_text(
                index, "min_kelvin", entity, "min_color_temp_kelvin"
            )
        if CONF_MAX_COLOR_TEMP_KELVIN_SENSOR in light:
            max_color_temp_kelvin = await cg.get_variable(light[CONF_MAX_COLOR_TEMP_KELVIN_SENSOR])
        else:
            max_color_temp_kelvin = await _make_ha_text(
                index, "max_kelvin", entity, "max_color_temp_kelvin"
            )
        cg.add(
            var.add_light(
                entity,
                name,
                state,
                modes,
                brightness,
                color,
                color_mode,
                color_temp_kelvin,
                min_color_temp_kelvin,
                max_color_temp_kelvin,
            )
        )
