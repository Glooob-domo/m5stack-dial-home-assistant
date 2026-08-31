import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

CONF_LANGUAGE = "language"

CODEOWNERS = []

LANGUAGES = {
    "en": 0,
    "fr": 1,
    "es": 2,
    "de": 3,
    "it": 4,
}

dial_i18n_ns = cg.esphome_ns.namespace("dial_i18n")
DialI18n = dial_i18n_ns.class_("DialI18n", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(DialI18n),
        cv.Required(CONF_LANGUAGE): cv.one_of(*LANGUAGES, lower=True),
    }
)


async def to_code(config):
    cg.add_define("DIAL_UI_LANG", LANGUAGES[config[CONF_LANGUAGE]])
    cg.add_global(cg.RawStatement('#include "esphome/components/dial_i18n/dial_i18n.h"'))
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
