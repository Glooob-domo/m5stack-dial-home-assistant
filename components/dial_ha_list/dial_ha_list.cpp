#include "dial_ha_list.h"

#include <cctype>
#include <cstdlib>

#include "esphome/core/log.h"

namespace esphome {
namespace dial_ha_list {

static const char *const TAG = "dial_ha_list";
namespace {
const std::string EMPTY_STRING;
}

bool DialHaList::value_valid_(const std::string &value) {
  if (value.empty())
    return false;
  std::string lower = value;
  for (char &c : lower)
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return lower != "unknown" && lower != "unavailable" && lower != "none" && lower != "null";
}

bool DialHaList::sensor_ready_(text_sensor::TextSensor *sensor) {
  return sensor != nullptr && (sensor->has_state() || !sensor->state.empty());
}

void DialHaList::add_entity(std::string entity_id, std::string name, text_sensor::TextSensor *state) {
  for (const auto &existing : this->entries_) {
    if (existing.entity_id == entity_id) {
      ESP_LOGW(TAG, "Duplicate entity_id ignored: %s", entity_id.c_str());
      return;
    }
  }
  Entry entry;
  entry.entity_id = std::move(entity_id);
  entry.name = std::move(name);
  entry.state = state;
  this->entries_.push_back(std::move(entry));
}

void DialHaList::add_attr(std::string key, text_sensor::TextSensor *sensor) {
  if (this->entries_.empty())
    return;
  Attr attr;
  attr.key = std::move(key);
  attr.sensor = sensor;
  this->entries_.back().attrs.push_back(std::move(attr));
}

void DialHaList::setup() {
  for (size_t i = 0; i < this->entries_.size(); i++) {
    auto &entry = this->entries_[i];
    if (entry.state != nullptr) {
      entry.state->add_on_state_callback([this, i](const std::string &value) { this->on_state_(i, value); });
      if (entry.state->has_state())
        this->on_state_(i, entry.state->state);
    }
    for (size_t a = 0; a < entry.attrs.size(); a++) {
      auto &attr = entry.attrs[a];
      if (attr.sensor == nullptr)
        continue;
      attr.sensor->add_on_state_callback([this, i, a](const std::string &value) { this->on_attr_(i, a, value); });
      if (attr.sensor->has_state())
        this->on_attr_(i, a, attr.sensor->state);
    }
  }
}

void DialHaList::loop() {
  for (size_t i = 0; i < this->entries_.size(); i++) {
    auto &entry = this->entries_[i];
    if (sensor_ready_(entry.state) && entry.state->state != entry.state_value)
      this->on_state_(i, entry.state->state);
    for (size_t a = 0; a < entry.attrs.size(); a++) {
      auto &attr = entry.attrs[a];
      if (sensor_ready_(attr.sensor) && attr.sensor->state != attr.value)
        this->on_attr_(i, a, attr.sensor->state);
    }
  }
}

void DialHaList::select(size_t index) {
  if (this->entries_.empty())
    return;
  if (index >= this->entries_.size())
    index = this->entries_.size() - 1;
  this->active_index_ = index;
}

void DialHaList::select_next() {
  if (this->entries_.empty())
    return;
  this->active_index_ = (this->active_index_ + 1) % this->entries_.size();
}

void DialHaList::select_previous() {
  if (this->entries_.empty())
    return;
  this->active_index_ = (this->active_index_ + this->entries_.size() - 1) % this->entries_.size();
}

const std::string &DialHaList::name_at(size_t index) const {
  if (index >= this->entries_.size())
    return EMPTY_STRING;
  return this->entries_[index].name;
}

const std::string &DialHaList::entity_id_at(size_t index) const {
  if (index >= this->entries_.size())
    return EMPTY_STRING;
  return this->entries_[index].entity_id;
}

const std::string &DialHaList::state_at(size_t index) const {
  if (index >= this->entries_.size())
    return EMPTY_STRING;
  const auto &entry = this->entries_[index];
  if (sensor_ready_(entry.state))
    return entry.state->state;
  return entry.state_value;
}

bool DialHaList::state_valid_at(size_t index) const {
  if (index >= this->entries_.size())
    return false;
  const auto &entry = this->entries_[index];
  if (sensor_ready_(entry.state))
    return value_valid_(entry.state->state);
  return entry.state_valid;
}

const std::string &DialHaList::attr_at(size_t index, const char *key) const {
  if (index >= this->entries_.size() || key == nullptr)
    return EMPTY_STRING;
  for (const auto &attr : this->entries_[index].attrs) {
    if (attr.key != key)
      continue;
    if (sensor_ready_(attr.sensor))
      return attr.sensor->state;
    return attr.value;
  }
  return EMPTY_STRING;
}

bool DialHaList::attr_valid_at(size_t index, const char *key) const {
  if (index >= this->entries_.size() || key == nullptr)
    return false;
  for (const auto &attr : this->entries_[index].attrs) {
    if (attr.key != key)
      continue;
    if (sensor_ready_(attr.sensor))
      return value_valid_(attr.sensor->state);
    return attr.valid;
  }
  return false;
}

int DialHaList::position_at(size_t index) const {
  const char *key = nullptr;
  if (this->attr_valid_at(index, "position"))
    key = "position";
  else if (this->attr_valid_at(index, "position_alt"))
    key = "position_alt";
  if (key == nullptr)
    return -1;
  int pos = atoi(this->attr_at(index, key).c_str());
  if (pos < 0)
    pos = 0;
  if (pos > 100)
    pos = 100;
  return pos;
}

bool DialHaList::cover_is_open_at(size_t index) const {
  if (this->state_valid_at(index)) {
    std::string st = this->state_at(index);
    for (char &c : st) {
      if (c >= 'A' && c <= 'Z')
        c = static_cast<char>(c - 'A' + 'a');
    }
    if (st == "open" || st == "opening" || st == "on")
      return true;
    if (st == "closed" || st == "closing" || st == "off")
      return false;
  }
  return this->position_at(index) > 0;
}

bool DialHaList::cover_known_at(size_t index) const {
  return this->state_valid_at(index) || this->position_at(index) >= 0;
}

void DialHaList::on_state_(size_t index, const std::string &value) {
  auto &entry = this->entries_[index];
  entry.state_value = value;
  entry.state_valid = value_valid_(value);
}

void DialHaList::on_attr_(size_t index, size_t attr_index, const std::string &value) {
  auto &attr = this->entries_[index].attrs[attr_index];
  attr.value = value;
  attr.valid = value_valid_(value);
}

}  // namespace dial_ha_list
}  // namespace esphome
