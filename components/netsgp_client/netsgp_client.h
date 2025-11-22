#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "AsyncNETSGPClient.h"
#include <memory>

namespace esphome {
namespace netsgp_client {

class NETSGPClientComponent : public PollingComponent, public uart::UARTDevice {
 public:
  NETSGPClientComponent();

  void set_prog_pin(InternalGPIOPin *pin) { prog_pin_ = pin; }
  void set_inverter_id(uint32_t inverter_id) { inverter_id_ = inverter_id; }
  void set_temperature_sensor(sensor::Sensor *sensor) { temperature_sensor_ = sensor; }
  void set_ac_power_sensor(sensor::Sensor *sensor) { ac_power_sensor_ = sensor; }
  void set_dc_power_sensor(sensor::Sensor *sensor) { dc_power_sensor_ = sensor; }
  void set_total_production_sensor(sensor::Sensor *sensor) { total_production_sensor_ = sensor; }

  void setup() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  static NETSGPClientComponent *instance_;
  static void status_callback(const AsyncNETSGPClient::InverterStatus &status);

  class StreamAdapter : public Stream {
   public:
    explicit StreamAdapter(uart::UARTDevice &device) : device_(device) {}
    size_t write(uint8_t data) override;
    int available() override;
    int read() override;
    int peek() override;
    void flush() override;

   protected:
    uart::UARTDevice &device_;
  };

  void handle_status(const AsyncNETSGPClient::InverterStatus &status);

  InternalGPIOPin *prog_pin_{nullptr};
  uint32_t inverter_id_{0};
  StreamAdapter stream_;
  std::unique_ptr<AsyncNETSGPClient> client_;

  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *ac_power_sensor_{nullptr};
  sensor::Sensor *dc_power_sensor_{nullptr};
  sensor::Sensor *total_production_sensor_{nullptr};
};

}  // namespace netsgp_client
}  // namespace esphome
