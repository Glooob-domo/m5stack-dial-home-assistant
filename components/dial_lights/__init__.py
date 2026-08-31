import esphome.codegen as cg
import esphome.config_validation as cv
import esphome.components.sensor as sensor
import esphome.components.text_sensor as text_sensor
from esphome.const import CONF_ATTRIBUTE, CONF_ENTITY_ID, CONF_ID, CONF_INTERNAL, CONF_NAME
from esphome.core import ID

CODEOWNERS = []
DEPENDENCIES = ["api", "sensor", "text_sensor"]
AUTO_LOAD = ["sensor", "text_sensor"]

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
    "climate.your_ac",
    "media_player.your_player",
    "weather.your_location",
}

dial_lights_ns = cg.esphome_ns.namespace("dial_lights")
DialLights = dial_lights_ns.class_("DialLights", cg.Component)

LIGHT_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ENTITY_ID): cv.string,
        cv.Required(CONF_NAME): cv.string,
        cv.Optional(CONF_STATE_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_MODES_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_BRIGHTNESS_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_COLOR_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_COLOR_MODE_SENSOR): cv.use_id(text_sensor.TextSensor),
        cv.Optional(CONF_COLOR_TEMP_KELVIN_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_MIN_COLOR_TEMP_KELVIN_SENSOR): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_MAX_COLOR_TEMP_KELVIN_SENSOR): cv.use_id(sensor.Sensor),
    }
)


def entity_enabled(entity):
    if not entity:
        return False
    if entity in LEGACY_DISABLED:
        return False
    if "." in entity and entity.rsplit(".", 1)[1] == "disabled":
        return False
    return True


def _config_schema(value):
    lights = cv.All(cv.ensure_list(LIGHT_SCHEMA), cv.Length(min=0))(value)
    return {
        CONF_ID: cv.declare_id(DialLights)("dial_lights_id"),
        "lights": lights,
    }


CONFIG_SCHEMA = _config_schema


def _ha_id(index, suffix, type_):
    return ID(f"dial_light_{index}_{suffix}", True, type_)


async def _make_ha_text(index, suffix, entity_id, attribute=None):
    from esphome.components.homeassistant import setup_home_assistant_entity
    from esphome.components.homeassistant.text_sensor import HomeassistantTextSensor

    uid = _ha_id(index, suffix, HomeassistantTextSensor)
    var = cg.new_Pvariable(uid)
    await text_sensor.register_text_sensor(var, {CONF_ID: uid, CONF_INTERNAL: True})
    await cg.register_component(var, {CONF_ID: uid})
    ha_conf = {CONF_ENTITY_ID: entity_id, CONF_INTERNAL: True}
    if attribute is not None:
        ha_conf[CONF_ATTRIBUTE] = attribute
    setup_home_assistant_entity(var, ha_conf)
    return var


async def _make_ha_sensor(index, suffix, entity_id, attribute):
    from esphome.components.homeassistant import setup_home_assistant_entity
    from esphome.components.homeassistant.sensor import HomeassistantSensor

    uid = _ha_id(index, suffix, HomeassistantSensor)
    var = cg.new_Pvariable(uid)
    await sensor.register_sensor(var, {CONF_ID: uid, CONF_INTERNAL: True})
    await cg.register_component(var, {CONF_ID: uid})
    setup_home_assistant_entity(
        var,
        {
            CONF_ENTITY_ID: entity_id,
            CONF_ATTRIBUTE: attribute,
            CONF_INTERNAL: True,
        },
    )
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
            brightness = await _make_ha_sensor(index, "brightness", entity, "brightness")
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
            color_temp_kelvin = await _make_ha_sensor(index, "kelvin", entity, "color_temp_kelvin")
        if CONF_MIN_COLOR_TEMP_KELVIN_SENSOR in light:
            min_color_temp_kelvin = await cg.get_variable(light[CONF_MIN_COLOR_TEMP_KELVIN_SENSOR])
        else:
            min_color_temp_kelvin = await _make_ha_sensor(
                index, "min_kelvin", entity, "min_color_temp_kelvin"
            )
        if CONF_MAX_COLOR_TEMP_KELVIN_SENSOR in light:
            max_color_temp_kelvin = await cg.get_variable(light[CONF_MAX_COLOR_TEMP_KELVIN_SENSOR])
        else:
            max_color_temp_kelvin = await _make_ha_sensor(
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
