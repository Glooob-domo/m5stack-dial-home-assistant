"""Sendspin Media Player Setup."""

from esphome import automation
import esphome.codegen as cg
from esphome.components import audio, esp32, network, socket, wifi
import esphome.config_validation as cv
from esphome.const import (
    CONF_BUFFER_SIZE,
    CONF_ID,
    CONF_TASK_STACK_IN_PSRAM,
    CONF_THEN,
    PLATFORM_ESP32,
)

# audio for codec support, json for protocol messages, mdns for autodiscovery
AUTO_LOAD = ["audio", "json", "mdns"]
CODEOWNERS = ["@kahrendt"]
DEPENDENCIES = ["network"]

CONF_ON_SERVER_SETTINGS = "on_server_settings"
CONF_KALMAN_PROCESS_ERROR = "kalman_process_error"
CONF_KALMAN_FORGET_FACTOR = "kalman_forget_factor"


CONF_SENDSPIN_ID = "sendspin_id"

sendspin_ns = cg.esphome_ns.namespace("sendspin")
SendspinHub = sendspin_ns.class_(
    "SendspinHub",
    cg.Component,
)


PublishClientSettingsAction = sendspin_ns.class_(
    "PublishClientSettingsAction",
    automation.Action,
    cg.Parented.template(SendspinHub),
)

SendSwitchCommandAction = sendspin_ns.class_(
    "SendSwitchCommandAction",
    automation.Action,
    cg.Parented.template(SendspinHub),
)

SendCommandAction = sendspin_ns.class_(
    "SendCommandAction",
    automation.Action,
    cg.Parented.template(SendspinHub),
)

SendVolumeCommandAction = sendspin_ns.class_(
    "SendVolumeCommandAction",
    automation.Action,
    cg.Parented.template(SendspinHub),
)

GetTrackProgressAction = sendspin_ns.class_(
    "GetTrackProgressAction",
    automation.Action,
    cg.Parented.template(SendspinHub),
)

SendspinControllerCommand = sendspin_ns.enum("SendspinControllerCommand", is_class=True)
SENDSPIN_CONTROLLER_COMMANDS = {
    "play": SendspinControllerCommand.PLAY,
    "pause": SendspinControllerCommand.PAUSE,
    "stop": SendspinControllerCommand.STOP,
    "next": SendspinControllerCommand.NEXT,
    "previous": SendspinControllerCommand.PREVIOUS,
    "repeat_off": SendspinControllerCommand.REPEAT_OFF,
    "repeat_one": SendspinControllerCommand.REPEAT_ONE,
    "repeat_all": SendspinControllerCommand.REPEAT_ALL,
    "shuffle": SendspinControllerCommand.SHUFFLE,
    "unshuffle": SendspinControllerCommand.UNSHUFFLE,
    "switch": SendspinControllerCommand.SWITCH,
}


def _request_high_performance_networking(config):
    """Request high performance networking for Sendspin streaming."""
    network.require_high_performance_networking()
    # Socket consumption varies by mode:
    # - Server mode: 1 listening socket + 2 client connections (for handoff)
    # - Client mode: 1 outbound connection
    socket.consume_sockets(3, "sendspin_websocket_server")(config)
    socket.consume_sockets(1, "sendspin_websocket_client")(config)

    wifi.enable_runtime_power_save_control()
    return config


CONFIG_SCHEMA = cv.All(
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(SendspinHub),
            cv.Optional(CONF_TASK_STACK_IN_PSRAM, default=False): cv.boolean,
            cv.Optional(CONF_KALMAN_PROCESS_ERROR): cv.invalid(
                f"The {CONF_KALMAN_PROCESS_ERROR} option has been removed"
            ),
            cv.Optional(CONF_KALMAN_FORGET_FACTOR): cv.invalid(
                f"The {CONF_KALMAN_FORGET_FACTOR} option has been removed"
            ),
            cv.Optional(CONF_BUFFER_SIZE): cv.invalid(
                f"The {CONF_BUFFER_SIZE} option is now set as an option in the Sendspin media_source configuration"
            ),
        }
    ),
    cv.only_on([PLATFORM_ESP32]),
    _request_high_performance_networking,
)


def _final_validate_codecs(config):
    audio.request_flac_support()
    audio.request_opus_support()
    return config


FINAL_VALIDATE_SCHEMA = _final_validate_codecs


async def to_code(config):
    cg.add_define("USE_SENDSPIN", True)  # for MDNS

    # Client mode - enable ESP-IDF WebSocket client
    esp32.add_idf_component(name="espressif/esp_websocket_client", ref="1.6.1")
    # Server mode - enable HTTP server with WebSocket support
    esp32.add_idf_sdkconfig_option("CONFIG_HTTPD_WS_SUPPORT", True)

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    if task_stack_in_psram := config.get(CONF_TASK_STACK_IN_PSRAM):
        cg.add(var.set_task_stack_in_psram(task_stack_in_psram))
        if task_stack_in_psram:
            esp32.add_idf_sdkconfig_option(
                "CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY", True
            )


SENDSPIN_SWITCH_ACTION_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(SendspinHub),
    }
)


@automation.register_action(
    "sendspin.switch",
    SendSwitchCommandAction,
    SENDSPIN_SWITCH_ACTION_SCHEMA,
    synchronous=True,
)
async def sendspin_switch_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    cg.add_define("USE_SENDSPIN_CONTROLLER")
    return var


SENDSPIN_COMMAND_ACTION_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(SendspinHub),
        cv.Required("command"): cv.templatable(cv.enum(SENDSPIN_CONTROLLER_COMMANDS)),
    }
)


@automation.register_action(
    "sendspin.command",
    SendCommandAction,
    SENDSPIN_COMMAND_ACTION_SCHEMA,
    synchronous=True,
)
async def sendspin_command_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    command = await cg.templatable(
        config["command"], args, SendspinControllerCommand
    )
    cg.add(var.set_command(command))
    cg.add_define("USE_SENDSPIN_CONTROLLER")
    return var


SENDSPIN_VOLUME_ACTION_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(SendspinHub),
        cv.Required("volume"): cv.templatable(cv.int_range(min=0, max=100)),
    }
)


@automation.register_action(
    "sendspin.volume_set",
    SendVolumeCommandAction,
    SENDSPIN_VOLUME_ACTION_SCHEMA,
    synchronous=True,
)
async def sendspin_volume_set_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    volume = await cg.templatable(config["volume"], args, cg.uint8)
    cg.add(var.set_volume(volume))
    cg.add_define("USE_SENDSPIN_CONTROLLER")
    return var


SENDSPIN_GET_TRACK_PROGRESS_ACTION_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(SendspinHub),
        cv.Required(CONF_THEN): automation.validate_action_list,
    }
)


@automation.register_action(
    "sendspin.get_track_progress",
    GetTrackProgressAction,
    SENDSPIN_GET_TRACK_PROGRESS_ACTION_SCHEMA,
    synchronous=True,
)
async def sendspin_get_track_progress_to_code(config, action_id, template_arg, args):
    cg.add_define("USE_SENDSPIN_METADATA", True)
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    actions = await automation.build_action_list(
        config[CONF_THEN],
        cg.TemplateArguments(cg.uint32, *template_arg.args),
        [(cg.uint32, "x"), *args],
    )
    cg.add(var.add_then(actions))
    return var
