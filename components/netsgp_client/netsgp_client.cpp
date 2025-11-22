#include "netsgp_client.h"
#include "esphome/core/log.h"
#include <inttypes.h>

namespace esphome {
namespace netsgp_client {

static const char *const TAG = "netsgp_client";

NETSGPClientComponent *NETSGPClientComponent::instance_{nullptr};

NETSGPClientComponent::NETSGPClientComponent()
    : PollingComponent(3000), uart::UARTDevice(), stream_{*this} {}

size_t NETSGPClientComponent::StreamAdapter::write(uint8_t data) {
  this->device_.write_byte(data);
  return 1;
}

int NETSGPClientComponent::StreamAdapter::available() {
  return this->device_.available();
}

int NETSGPClientComponent::StreamAdapter::read() {
  return this->device_.read();
}

int NETSGPClientComponent::StreamAdapter::peek() {
  return this->device_.peek();
}

void NETSGPClientComponent::StreamAdapter::flush() {
  this->device_.flush();
}

void NETSGPClientComponent::setup() {
  instance_ = this;

  if (this->prog_pin_ != nullptr) {
    this->prog_pin_->setup();
    this->prog_pin_->pin_mode(gpio::FLAG_OUTPUT);
    this->prog_pin_->digital_write(false);
  }

  const uint8_t prog_pin_number = this->prog_pin_ != nullptr ? this->prog_pin_->get_pin() : 0;
  this->client_ = std::make_unique<AsyncNETSGPClient>(this->stream_, prog_pin_number);

  this->client_->setDefaultRFSettings();
  this->client_->setStatusCallback(&NETSGPClientComponent::status_callback);
  this->client_->registerInverter(this->inverter_id_);

  ESP_LOGCONFIG(TAG, "NETSGPClient initialized for inverter 0x%08" PRIx32, this->inverter_id_);
}

void NETSGPClientComponent::status_callback(const AsyncNETSGPClient::InverterStatus &status) {
  if (instance_ != nullptr) {
    instance_->handle_status(status);
  }
}

void NETSGPClientComponent::handle_status(const AsyncNETSGPClient::InverterStatus &status) {
  if (status.deviceID != this->inverter_id_) {
    return;
  }

  if (this->temperature_sensor_ != nullptr) {
    this->temperature_sensor_->publish_state(status.temperature);
  }
  if (this->ac_power_sensor_ != nullptr) {
    this->ac_power_sensor_->publish_state(status.acPower);
  }
  if (this->dc_power_sensor_ != nullptr) {
    this->dc_power_sensor_->publish_state(status.dcPower);
  }
  if (this->total_production_sensor_ != nullptr) {
    this->total_production_sensor_->publish_state(status.totalGeneratedPower);
  }
}

void NETSGPClientComponent::update() {
  if (this->client_ != nullptr) {
    this->client_->update();
  }
}

}  // namespace netsgp_client
}  // namespace esphome
