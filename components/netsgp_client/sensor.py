import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor, uart
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_TEMPERATURE,
    ICON_FLASH,
    ICON_THERMOMETER,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_CELSIUS,
    UNIT_KILOWATT_HOURS,
    UNIT_WATT,
)

CONF_INVERTER_ID = "inverter_id"
CONF_PROG_PIN = "prog_pin"
CONF_TEMPERATURE = "temperature"
CONF_AC_POWER = "ac_power"
CONF_DC_POWER = "dc_power"
CONF_TOTAL_PRODUCTION = "total_production"

AUTO_LOAD = ["sensor", "uart"]
DEPENDENCIES = ["uart"]

netsgp_client_ns = cg.esphome_ns.namespace("netsgp_client")
NETSGPClientComponent = netsgp_client_ns.class_(
    "NETSGPClientComponent", cg.PollingComponent, uart.UARTDevice
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(NETSGPClientComponent),
        cv.Required(CONF_INVERTER_ID): cv.uint32_t,
        cv.Required(CONF_PROG_PIN): pins.gpio_output_pin_schema,
        cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_THERMOMETER,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_AC_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_FLASH,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DC_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_FLASH,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TOTAL_PRODUCTION): sensor.sensor_schema(
            unit_of_measurement=UNIT_KILOWATT_HOURS,
            icon=ICON_FLASH,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_ENERGY,
            state_class=STATE_CLASS_TOTAL_INCREASING,
        ),
    }
).extend(uart.UART_DEVICE_SCHEMA).extend(cv.polling_component_schema("3s"))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    cg.add(var.set_inverter_id(config[CONF_INVERTER_ID]))

    prog_pin = await cg.gpio_pin_expression(config[CONF_PROG_PIN])
    cg.add(var.set_prog_pin(prog_pin))

    if CONF_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_TEMPERATURE])
        cg.add(var.set_temperature_sensor(sens))
    if CONF_AC_POWER in config:
        sens = await sensor.new_sensor(config[CONF_AC_POWER])
        cg.add(var.set_ac_power_sensor(sens))
    if CONF_DC_POWER in config:
        sens = await sensor.new_sensor(config[CONF_DC_POWER])
        cg.add(var.set_dc_power_sensor(sens))
    if CONF_TOTAL_PRODUCTION in config:
        sens = await sensor.new_sensor(config[CONF_TOTAL_PRODUCTION])
        cg.add(var.set_total_production_sensor(sens))
