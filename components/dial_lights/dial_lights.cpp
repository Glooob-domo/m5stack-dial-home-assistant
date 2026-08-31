#include "dial_lights.h"

#include <cctype>
#include <cmath>
#include <cstdlib>

#include "esphome/core/log.h"

namespace esphome {
namespace dial_lights {

static const char *const TAG = "dial_lights";

namespace {
const std::string EMPTY_STRING;
constexpr int DEFAULT_MIN_KELVIN = 2000;
constexpr int DEFAULT_MAX_KELVIN = 6500;

void parse_color_modes(const std::string &raw, bool &supports_brightness, bool &supports_rgb,
                       bool &supports_color_temp) {
  supports_brightness = false;
  supports_rgb = false;
  supports_color_temp = false;

  std::string token;
  for (size_t i = 0; i <= raw.size(); i++) {
    const char c = (i < raw.size()) ? raw[i] : '\0';
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_') {
      token += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    } else if (!token.empty()) {
      if (token == "brightness" || token == "color_temp" || token == "hs" || token == "xy" || token == "rgb" ||
          token == "rgbw" || token == "rgbww") {
        supports_brightness = true;
      }
      if (token == "rgb" || token == "rgbw" || token == "rgbww" || token == "hs" || token == "xy") {
        supports_rgb = true;
      }
      if (token == "color_temp") {
        supports_color_temp = true;
      }
      token.clear();
    }
  }
}

bool parse_brightness_percent(float raw, int &percent) {
  if (!std::isfinite(raw))
    return false;
  float clamped = raw;
  if (clamped < 0.0f)
    clamped = 0.0f;
  else if (clamped > 255.0f)
    clamped = 255.0f;
  percent = static_cast<int>(clamped * 100.0f / 255.0f + 0.5f);
  if (percent < 0)
    percent = 0;
  else if (percent > 100)
    percent = 100;
  return true;
}

void trim_string(std::string &text) {
  while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front())))
    text.erase(text.begin());
  while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back())))
    text.pop_back();
}

bool parse_ha_float(const std::string &raw, float &value) {
  std::string text = raw;
  trim_string(text);
  if (text.empty())
    return false;
  if (text == "None" || text == "none" || text == "null" || text == "unknown" || text == "unavailable")
    return false;
  char *end = nullptr;
  const float parsed = std::strtof(text.c_str(), &end);
  if (end == text.c_str())
    return false;
  while (end != nullptr && *end != '\0' && std::isspace(static_cast<unsigned char>(*end)))
    end++;
  if (end == nullptr || *end != '\0')
    return false;
  if (!std::isfinite(parsed))
    return false;
  value = parsed;
  return true;
}

bool parse_int_strict(const std::string &text, size_t &pos, int &value) {
  while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos])))
    pos++;
  if (pos >= text.size())
    return false;
  char *end = nullptr;
  const long parsed = std::strtol(text.c_str() + pos, &end, 10);
  if (end == text.c_str() + pos)
    return false;
  value = static_cast<int>(parsed);
  pos = static_cast<size_t>(end - text.c_str());
  return true;
}

bool parse_rgb_color(const std::string &raw, int &r, int &g, int &b) {
  std::string text = raw;
  trim_string(text);
  if (text.empty())
    return false;

  const char open = text.front();
  char close = '\0';
  if (open == '[')
    close = ']';
  else if (open == '(')
    close = ')';
  else
    return false;

  if (text.back() != close)
    return false;

  const std::string inner = text.substr(1, text.size() - 2);
  size_t pos = 0;
  int ri = 0;
  int gi = 0;
  int bi = 0;
  if (!parse_int_strict(inner, pos, ri))
    return false;
  while (pos < inner.size() && std::isspace(static_cast<unsigned char>(inner[pos])))
    pos++;
  if (pos >= inner.size() || inner[pos] != ',')
    return false;
  pos++;
  if (!parse_int_strict(inner, pos, gi))
    return false;
  while (pos < inner.size() && std::isspace(static_cast<unsigned char>(inner[pos])))
    pos++;
  if (pos >= inner.size() || inner[pos] != ',')
    return false;
  pos++;
  if (!parse_int_strict(inner, pos, bi))
    return false;
  while (pos < inner.size() && std::isspace(static_cast<unsigned char>(inner[pos])))
    pos++;
  if (pos != inner.size())
    return false;

  r = std::max(0, std::min(255, ri));
  g = std::max(0, std::min(255, gi));
  b = std::max(0, std::min(255, bi));
  return true;
}

int rgb_to_hue(int r, int g, int b) {
  float rf = r / 255.0f;
  float gf = g / 255.0f;
  float bf = b / 255.0f;
  float maxc = std::max(rf, std::max(gf, bf));
  float minc = std::min(rf, std::min(gf, bf));
  float delta = maxc - minc;
  float hue = 0.0f;
  if (delta > 0.0001f) {
    if (maxc == rf)
      hue = 60.0f * std::fmod((gf - bf) / delta, 6.0f);
    else if (maxc == gf)
      hue = 60.0f * (((bf - rf) / delta) + 2.0f);
    else
      hue = 60.0f * (((rf - gf) / delta) + 4.0f);
  }
  if (hue < 0.0f)
    hue += 360.0f;
  return static_cast<int>(hue + 0.5f) % 360;
}

bool parse_kelvin_value(float raw, int &kelvin) {
  if (!std::isfinite(raw) || raw <= 0.0f)
    return false;
  kelvin = static_cast<int>(raw + 0.5f);
  if (kelvin < 1000)
    kelvin = 1000;
  else if (kelvin > 100000)
    kelvin = 100000;
  return true;
}

void sanitize_kelvin_range(int &min_k, int &max_k) {
  if (min_k <= 0)
    min_k = DEFAULT_MIN_KELVIN;
  if (max_k <= 0)
    max_k = DEFAULT_MAX_KELVIN;
  if (min_k > max_k) {
    const int tmp = min_k;
    min_k = max_k;
    max_k = tmp;
  }
  if (max_k - min_k < 1)
    max_k = min_k + 1;
}

int clamp_kelvin(int kelvin, int min_k, int max_k) {
  sanitize_kelvin_range(min_k, max_k);
  if (kelvin < min_k)
    return min_k;
  if (kelvin > max_k)
    return max_k;
  return kelvin;
}
}  // namespace

void DialLights::add_light(const std::string &entity_id, const std::string &name, text_sensor::TextSensor *state,
                           text_sensor::TextSensor *modes, text_sensor::TextSensor *brightness,
                           text_sensor::TextSensor *color, text_sensor::TextSensor *color_mode,
                           text_sensor::TextSensor *color_temp_kelvin, text_sensor::TextSensor *min_color_temp_kelvin,
                           text_sensor::TextSensor *max_color_temp_kelvin) {
  for (const auto &existing : this->lights_) {
    if (existing.entity_id == entity_id) {
      ESP_LOGW(TAG, "Duplicate light entity_id ignored: %s", entity_id.c_str());
      return;
    }
  }
  this->lights_.push_back({entity_id,
                           name,
                           state,
                           false,
                           false,
                           modes,
                           false,
                           false,
                           false,
                           brightness,
                           false,
                           75,
                           color,
                           false,
                           169,
                           143,
                           255,
                           color_mode,
                           false,
                           false,
                           color_temp_kelvin,
                           false,
                           4000,
                           min_color_temp_kelvin,
                           false,
                           DEFAULT_MIN_KELVIN,
                           max_color_temp_kelvin,
                           false,
                           DEFAULT_MAX_KELVIN});
}

void DialLights::setup() {
  for (size_t i = 0; i < this->lights_.size(); i++) {
    auto &light = this->lights_[i];
    if (light.state != nullptr) {
      light.state->add_on_state_callback([this, i](const std::string &value) { this->on_state_(i, value); });
      if (light.state->has_state()) {
        this->on_state_(i, light.state->state);
      }
    }
    if (light.modes != nullptr) {
      light.modes->add_on_state_callback([this, i](const std::string &value) { this->on_modes_(i, value); });
      if (light.modes->has_state()) {
        this->on_modes_(i, light.modes->state);
      }
    }
    if (light.brightness != nullptr) {
      light.brightness->add_on_state_callback([this, i](const std::string &value) { this->on_brightness_(i, value); });
      if (light.brightness->has_state()) {
        this->on_brightness_(i, light.brightness->state);
      }
    }
    if (light.color != nullptr) {
      light.color->add_on_state_callback([this, i](const std::string &value) { this->on_color_(i, value); });
      if (light.color->has_state()) {
        this->on_color_(i, light.color->state);
      }
    }
    if (light.color_mode != nullptr) {
      light.color_mode->add_on_state_callback([this, i](const std::string &value) { this->on_color_mode_(i, value); });
      if (light.color_mode->has_state()) {
        this->on_color_mode_(i, light.color_mode->state);
      }
    }
    if (light.color_temp_kelvin != nullptr) {
      light.color_temp_kelvin->add_on_state_callback(
          [this, i](const std::string &value) { this->on_color_temp_kelvin_(i, value); });
      if (light.color_temp_kelvin->has_state()) {
        this->on_color_temp_kelvin_(i, light.color_temp_kelvin->state);
      }
    }
    if (light.min_color_temp_kelvin != nullptr) {
      light.min_color_temp_kelvin->add_on_state_callback(
          [this, i](const std::string &value) { this->on_min_color_temp_kelvin_(i, value); });
      if (light.min_color_temp_kelvin->has_state()) {
        this->on_min_color_temp_kelvin_(i, light.min_color_temp_kelvin->state);
      }
    }
    if (light.max_color_temp_kelvin != nullptr) {
      light.max_color_temp_kelvin->add_on_state_callback(
          [this, i](const std::string &value) { this->on_max_color_temp_kelvin_(i, value); });
      if (light.max_color_temp_kelvin->has_state()) {
        this->on_max_color_temp_kelvin_(i, light.max_color_temp_kelvin->state);
      }
    }
  }
}

void DialLights::load_active_snapshot() {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return;

  auto &light = this->lights_[this->active_index_];

  light.state_valid = false;
  light.is_on = false;
  light.supports_brightness = false;
  light.supports_rgb = false;
  light.supports_color_temp = false;

  if (light.state != nullptr && light.state->has_state()) {
    const std::string &value = light.state->state;
    if (value == "on") {
      light.state_valid = true;
      light.is_on = true;
    } else if (value == "off") {
      light.state_valid = true;
      light.is_on = false;
    }
  }

  if (light.modes != nullptr && light.modes->has_state()) {
    parse_color_modes(light.modes->state, light.supports_brightness, light.supports_rgb, light.supports_color_temp);
  }

  if (light.brightness != nullptr && light.brightness->has_state()) {
    this->on_brightness_(this->active_index_, light.brightness->state);
  }

  if (light.color != nullptr && light.color->has_state()) {
    int r = 0;
    int g = 0;
    int b = 0;
    if (parse_rgb_color(light.color->state, r, g, b)) {
      light.color_valid = true;
      light.color_r = r;
      light.color_g = g;
      light.color_b = b;
    }
  }

  if (light.color_mode != nullptr && light.color_mode->has_state()) {
    this->on_color_mode_(this->active_index_, light.color_mode->state);
  }

  if (light.color_temp_kelvin != nullptr && light.color_temp_kelvin->has_state()) {
    this->on_color_temp_kelvin_(this->active_index_, light.color_temp_kelvin->state);
  }

  if (light.min_color_temp_kelvin != nullptr && light.min_color_temp_kelvin->has_state()) {
    this->on_min_color_temp_kelvin_(this->active_index_, light.min_color_temp_kelvin->state);
  }

  if (light.max_color_temp_kelvin != nullptr && light.max_color_temp_kelvin->has_state()) {
    this->on_max_color_temp_kelvin_(this->active_index_, light.max_color_temp_kelvin->state);
  }
}

const DialLights::LightEntry &DialLights::active_entry_() const { return this->lights_[this->active_index_]; }

void DialLights::on_state_(size_t index, const std::string &value) {
  auto &light = this->lights_[index];
  if (value == "on") {
    light.state_valid = true;
    light.is_on = true;
    return;
  }
  if (value == "off") {
    light.state_valid = true;
    light.is_on = false;
    return;
  }
  light.state_valid = false;
}

void DialLights::on_modes_(size_t index, const std::string &value) {
  auto &light = this->lights_[index];
  light.supports_brightness = false;
  light.supports_rgb = false;
  light.supports_color_temp = false;
  parse_color_modes(value, light.supports_brightness, light.supports_rgb, light.supports_color_temp);
}

void DialLights::on_brightness_(size_t index, const std::string &value) {
  auto &light = this->lights_[index];
  float raw = 0.0f;
  int percent = 0;
  if (parse_ha_float(value, raw) && parse_brightness_percent(raw, percent)) {
    light.brightness_valid = true;
    light.brightness_percent = percent;
    return;
  }
  light.brightness_valid = false;
}

void DialLights::on_color_(size_t index, const std::string &value) {
  auto &light = this->lights_[index];
  int r = 0;
  int g = 0;
  int b = 0;
  if (parse_rgb_color(value, r, g, b)) {
    light.color_valid = true;
    light.color_r = r;
    light.color_g = g;
    light.color_b = b;
  }
}

void DialLights::on_color_mode_(size_t index, const std::string &value) {
  auto &light = this->lights_[index];
  if (value.empty()) {
    light.color_mode_valid = false;
    light.is_color_temp_mode = false;
    return;
  }
  light.color_mode_valid = true;
  light.is_color_temp_mode = (value == "color_temp");
}

void DialLights::on_color_temp_kelvin_(size_t index, const std::string &value) {
  auto &light = this->lights_[index];
  float raw = 0.0f;
  int kelvin = 0;
  if (!parse_ha_float(value, raw) || !parse_kelvin_value(raw, kelvin)) {
    light.color_temp_valid = false;
    return;
  }
  int min_k = light.min_color_temp_kelvin_valid ? light.min_color_temp_kelvin_value : DEFAULT_MIN_KELVIN;
  int max_k = light.max_color_temp_kelvin_valid ? light.max_color_temp_kelvin_value : DEFAULT_MAX_KELVIN;
  light.color_temp_valid = true;
  light.color_temp_kelvin_value = clamp_kelvin(kelvin, min_k, max_k);
}

void DialLights::on_min_color_temp_kelvin_(size_t index, const std::string &value) {
  auto &light = this->lights_[index];
  float raw = 0.0f;
  int kelvin = 0;
  if (parse_ha_float(value, raw) && parse_kelvin_value(raw, kelvin)) {
    light.min_color_temp_kelvin_valid = true;
    light.min_color_temp_kelvin_value = kelvin;
  }
}

void DialLights::on_max_color_temp_kelvin_(size_t index, const std::string &value) {
  auto &light = this->lights_[index];
  float raw = 0.0f;
  int kelvin = 0;
  if (parse_ha_float(value, raw) && parse_kelvin_value(raw, kelvin)) {
    light.max_color_temp_kelvin_valid = true;
    light.max_color_temp_kelvin_value = kelvin;
  }
}

bool DialLights::active_has_valid_state() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return false;
  return this->active_entry_().state_valid;
}

bool DialLights::active_is_on() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return false;
  return this->active_entry_().is_on;
}

bool DialLights::active_supports_brightness() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return false;
  return this->active_entry_().supports_brightness;
}

bool DialLights::active_supports_rgb() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return false;
  return this->active_entry_().supports_rgb;
}

bool DialLights::active_supports_color_temp() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return false;
  return this->active_entry_().supports_color_temp;
}

bool DialLights::active_brightness_valid() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return false;
  return this->active_entry_().brightness_valid;
}

int DialLights::active_brightness_percent() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return 0;
  return this->active_entry_().brightness_percent;
}

bool DialLights::active_color_valid() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return false;
  return this->active_entry_().color_valid;
}

int DialLights::active_color_r() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return 0;
  return this->active_entry_().color_r;
}

int DialLights::active_color_g() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return 0;
  return this->active_entry_().color_g;
}

int DialLights::active_color_b() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return 0;
  return this->active_entry_().color_b;
}

int DialLights::active_color_h() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return 0;
  const auto &light = this->active_entry_();
  if (!light.color_valid)
    return 0;
  return rgb_to_hue(light.color_r, light.color_g, light.color_b);
}

bool DialLights::active_color_mode_valid() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return false;
  return this->active_entry_().color_mode_valid;
}

bool DialLights::active_is_color_temp_mode() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return false;
  return this->active_entry_().is_color_temp_mode;
}

bool DialLights::active_color_temp_valid() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return false;
  return this->active_entry_().color_temp_valid;
}

int DialLights::active_color_temp_kelvin() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return DEFAULT_MIN_KELVIN;
  return this->active_entry_().color_temp_kelvin_value;
}

int DialLights::active_min_color_temp_kelvin() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return DEFAULT_MIN_KELVIN;
  const auto &light = this->active_entry_();
  int min_k = light.min_color_temp_kelvin_valid ? light.min_color_temp_kelvin_value : DEFAULT_MIN_KELVIN;
  int max_k = light.max_color_temp_kelvin_valid ? light.max_color_temp_kelvin_value : DEFAULT_MAX_KELVIN;
  sanitize_kelvin_range(min_k, max_k);
  return min_k;
}

int DialLights::active_max_color_temp_kelvin() const {
  if (this->lights_.empty() || this->active_index_ >= this->lights_.size())
    return DEFAULT_MAX_KELVIN;
  const auto &light = this->active_entry_();
  int min_k = light.min_color_temp_kelvin_valid ? light.min_color_temp_kelvin_value : DEFAULT_MIN_KELVIN;
  int max_k = light.max_color_temp_kelvin_valid ? light.max_color_temp_kelvin_value : DEFAULT_MAX_KELVIN;
  sanitize_kelvin_range(min_k, max_k);
  return max_k;
}

void DialLights::select_light(size_t index) {
  if (this->lights_.empty())
    return;
  this->active_index_ = index % this->lights_.size();
}

void DialLights::select_next() {
  if (!this->lights_.empty())
    this->active_index_ = (this->active_index_ + 1) % this->lights_.size();
}

void DialLights::select_previous() {
  if (!this->lights_.empty())
    this->active_index_ = (this->active_index_ + this->lights_.size() - 1) % this->lights_.size();
}

const std::string &DialLights::active_name() const { return this->name_at(this->active_index_); }

const std::string &DialLights::active_entity_id() const {
  if (this->lights_.empty())
    return EMPTY_STRING;
  return this->lights_[this->active_index_].entity_id;
}

const std::string &DialLights::name_at(size_t index) const {
  if (this->lights_.empty())
    return EMPTY_STRING;
  return this->lights_[index % this->lights_.size()].name;
}

LightCardSnapshot DialLights::card_snapshot_at(size_t index) const {
  LightCardSnapshot snap;
  if (this->lights_.empty() || index >= this->lights_.size())
    return snap;

  const auto &light = this->lights_[index];
  snap.name = light.name;
  snap.state_valid = light.state_valid;
  snap.is_on = light.is_on;
  snap.supports_brightness = light.supports_brightness;
  snap.supports_rgb = light.supports_rgb;
  snap.supports_color_temp = light.supports_color_temp;
  snap.brightness_valid = light.brightness_valid;
  snap.brightness_percent = light.brightness_percent;
  snap.color_valid = light.color_valid;
  snap.color_r = light.color_r;
  snap.color_g = light.color_g;
  snap.color_b = light.color_b;
  snap.color_mode_valid = light.color_mode_valid;
  snap.is_color_temp_mode = light.is_color_temp_mode;
  snap.color_temp_valid = light.color_temp_valid;
  snap.color_temp_kelvin = light.color_temp_kelvin_value;
  return snap;
}

}  // namespace dial_lights
}  // namespace esphome
