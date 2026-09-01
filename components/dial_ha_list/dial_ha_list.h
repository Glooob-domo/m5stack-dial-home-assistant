#pragma once

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace dial_ha_list {

class DialHaList : public Component {
 public:
  void add_entity(std::string entity_id, std::string name, text_sensor::TextSensor *state);
  void add_attr(std::string key, text_sensor::TextSensor *sensor);
  void add_num_attr(std::string key, sensor::Sensor *sensor);
  void setup() override;
  void loop() override;

  size_t count() const { return this->entries_.size(); }
  size_t active_index() const { return this->active_index_; }
  void select(size_t index);
  void select_next();
  void select_previous();

  const std::string &name_at(size_t index) const;
  const std::string &entity_id_at(size_t index) const;
  const std::string &state_at(size_t index) const;
  bool state_valid_at(size_t index) const;
  const std::string &attr_at(size_t index, const char *key) const;
  bool attr_valid_at(size_t index, const char *key) const;
  int position_at(size_t index) const;
  bool cover_is_open_at(size_t index) const;
  bool cover_known_at(size_t index) const;

  const std::string &active_name() const { return this->name_at(this->active_index_); }
  const std::string &active_entity_id() const { return this->entity_id_at(this->active_index_); }
  const std::string &active_state() const { return this->state_at(this->active_index_); }
  bool active_state_valid() const { return this->state_valid_at(this->active_index_); }
  const std::string &active_attr(const char *key) const { return this->attr_at(this->active_index_, key); }
  bool active_attr_valid(const char *key) const { return this->attr_valid_at(this->active_index_, key); }
  int active_position() const { return this->position_at(this->active_index_); }

 protected:
  struct Attr {
    std::string key;
    text_sensor::TextSensor *sensor{nullptr};
    std::string value;
    bool valid{false};
  };
  struct NumAttr {
    std::string key;
    sensor::Sensor *sensor{nullptr};
    float value{0.0f};
    bool has_value{false};
  };
  struct Entry {
    std::string entity_id;
    std::string name;
    text_sensor::TextSensor *state{nullptr};
    std::string state_value;
    bool state_valid{false};
    std::vector<Attr> attrs;
    std::vector<NumAttr> num_attrs;
    int cached_position{-1};
    bool cached_from_attr_{false};
  };

  void on_state_(size_t index, const std::string &value);
  void on_attr_(size_t index, size_t attr_index, const std::string &value);
  void on_num_(size_t index, size_t attr_index, float raw);
  void note_position_(size_t index, int pos, bool from_attr);
  int live_position_(size_t index) const;
  static bool value_valid_(const std::string &value);
  static bool sensor_ready_(text_sensor::TextSensor *sensor);
  static bool parse_percent_(const std::string &raw, int &pos);
  static bool parse_numeric_(float raw, int &pos);
  static bool is_position_key_(const std::string &key);

  std::vector<Entry> entries_;
  size_t active_index_{0};
};

}  // namespace dial_ha_list
}  // namespace esphome
