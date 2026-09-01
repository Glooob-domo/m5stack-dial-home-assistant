#include "dial_carousel.h"

#include <cmath>

namespace esphome {
namespace dial_carousel {

namespace {
constexpr float kCy = 120.0f;
constexpr float kArcCx = 162.0f;
constexpr float kR = 98.0f;
constexpr int kOffsets[5] = {-2, -1, 0, 1, 2};

float arc_dy(float off) {
  const float s = (off >= 0.0f) ? 1.0f : -1.0f;
  const float a = fabsf(off);
  return s * (a <= 1.0f ? a * 60.0f : 60.0f + (a - 1.0f) * 36.0f);
}
}  // namespace

void DialCarousel::set_slot(size_t index, lv_obj_t *container, lv_obj_t *icon, lv_obj_t *title) {
  if (index >= this->slots_.size())
    return;
  this->slots_[index].container = container;
  this->slots_[index].icon = icon;
  this->slots_[index].title = title;
}

CarouselSlotLayout DialCarousel::compute_layout_(size_t slot_index, float off, int pill_shift_max) const {
  CarouselSlotLayout layout{};

  float dy = arc_dy(off);
  if (dy > kR)
    dy = kR;
  if (dy < -kR)
    dy = -kR;

  const float theta = asinf(dy / kR);
  const float x = kArcCx - kR * cosf(theta);
  const float y = kCy + kR * sinf(theta);

  float closeness = 1.0f - (fabsf(off) / 2.0f);
  if (closeness < 0.0f)
    closeness = 0.0f;
  if (closeness > 1.0f)
    closeness = 1.0f;

  const int mix = static_cast<int>(closeness * 1000.0f);
  int w = 40;
  int opa_text = 0;
  int opa_bg = 0;
  int pill_shift = 0;

  if (mix > 760) {
    w = lv_map(mix, 760, 1000, 40, 168);
  }
  if (mix > 840) {
    opa_text = lv_map(mix, 840, 1000, 0, 255);
  }
  if (mix > 820) {
    opa_bg = lv_map(mix, 820, 1000, 0, 255);
  }
  if (mix > 760) {
    pill_shift = lv_map(mix, 760, 1000, 0, pill_shift_max);
  }

  layout.w = w;
  layout.opa_text = opa_text;
  layout.opa_bg = opa_bg;
  layout.opa_icon = lv_map(mix, 0, 1000, 96, 255);
  if (slot_index == 2) {
    layout.opa_title = opa_text;
  } else if (this->lateral_title_opa_max_ > 0) {
    layout.opa_title = this->compute_lateral_title_opa_(mix);
  } else {
    layout.opa_title = opa_text;
  }
  layout.x = static_cast<int>(x - 14.0f - 120.0f + static_cast<float>(pill_shift));
  layout.y = static_cast<int>(y - (slot_index == 2 ? 28.0f : 20.0f));
  return layout;
}

int DialCarousel::compute_lateral_title_opa_(int mix) const {
  if (this->lateral_title_opa_max_ <= 0 || mix <= 0)
    return 0;
  if (mix >= 1000)
    return 255;
  if (mix <= 500) {
    return lv_map(mix, 0, 500, 0, this->lateral_title_opa_max_);
  }
  return lv_map(mix, 500, 1000, this->lateral_title_opa_max_, 255);
}

CarouselSlotLayout DialCarousel::compute_static_layout(size_t slot_index, int pill_shift_max) const {
  if (slot_index >= this->slots_.size())
    return {};
  return this->compute_layout_(slot_index, static_cast<float>(kOffsets[slot_index]), pill_shift_max);
}

void DialCarousel::apply_container_geometry(size_t slot_index, const CarouselSlotLayout &layout) const {
  const auto &refs = this->slots_[slot_index];
  if (refs.container == nullptr)
    return;

  lv_obj_set_width(refs.container, layout.w);
  lv_obj_set_height(refs.container, slot_index == 2 ? 56 : 40);
  lv_obj_set_style_radius(refs.container, slot_index == 2 ? 28 : 20, LV_PART_MAIN);
  lv_obj_set_pos(refs.container, layout.x, layout.y);

  if (slot_index == 2 && this->center_bar_ != nullptr) {
    lv_obj_set_pos(this->center_bar_, layout.x - 10, layout.y + 12);
  }
}

void DialCarousel::apply_title_layout(size_t slot_index, const CarouselSlotLayout &layout) const {
  const auto &refs = this->slots_[slot_index];
  if (refs.title == nullptr)
    return;

  if (this->titles_are_overlays_ && slot_index != 2) {
    lv_obj_t *parent = lv_obj_get_parent(refs.title);
    if (parent == nullptr)
      return;

    const int slot_h = 40;
    const lv_font_t *font = lv_obj_get_style_text_font(refs.title, LV_PART_MAIN);
    const int line_h = font != nullptr ? lv_font_get_line_height(font) : 12;
    const int16_t parent_width = lv_obj_get_width(parent);
    if (parent_width <= 0)
      return;

    const int16_t title_x = ((parent_width - layout.w) / 2) + layout.x + 38;
    const int16_t title_y = layout.y + (slot_h - line_h) / 2;
    lv_obj_set_pos(refs.title, title_x, title_y);
  }

  lv_obj_set_style_opa(refs.title, layout.opa_title, LV_PART_MAIN);
}

void DialCarousel::apply_animation_frame(int32_t progress, CarouselDirection direction, int pill_shift_max) {
  const float phase = static_cast<float>(progress) / 100.0f;
  int center_sub_opa = 0;

  for (size_t slot_index = 0; slot_index < this->slots_.size(); slot_index++) {
    float off = static_cast<float>(kOffsets[slot_index]);
    if (direction == CarouselDirection::UP) {
      off += phase;
    } else {
      off -= phase;
    }

    const CarouselSlotLayout layout = this->compute_layout_(slot_index, off, pill_shift_max);
    const auto &refs = this->slots_[slot_index];
    if (refs.container == nullptr)
      continue;

    lv_obj_set_width(refs.container, layout.w);
    lv_obj_set_height(refs.container, slot_index == 2 ? 56 : 40);
    lv_obj_set_style_radius(refs.container, slot_index == 2 ? 28 : 20, LV_PART_MAIN);
    lv_obj_set_style_bg_color(refs.container, lv_color_hex(0x0D1537), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(refs.container, layout.opa_bg, LV_PART_MAIN);

    this->apply_container_geometry(slot_index, layout);

    if (refs.icon != nullptr) {
      lv_obj_set_style_opa(refs.icon, layout.opa_icon, LV_PART_MAIN);
    }
    this->apply_title_layout(slot_index, layout);

    if (slot_index == 2) {
      center_sub_opa = layout.opa_text;
    }
  }

  if (this->center_sub_ != nullptr) {
    lv_obj_set_style_opa(this->center_sub_, center_sub_opa, LV_PART_MAIN);
  }
}

bool DialCarousel::start_animation(CarouselDirection direction, int pill_shift_max, void (*on_ready)(void *),
                                   void *context) {
  if (this->animating_)
    return false;

  this->animating_ = true;
  this->anim_direction_ = direction;
  this->anim_pill_shift_max_ = pill_shift_max;
  this->on_ready_ = on_ready;
  this->on_ready_context_ = context;

  lv_anim_init(&this->anim_);
  lv_anim_set_var(&this->anim_, this);
  lv_anim_set_values(&this->anim_, 0, 100);
  lv_anim_set_time(&this->anim_, 200);
  lv_anim_set_path_cb(&this->anim_, lv_anim_path_ease_in_out);
  lv_anim_set_exec_cb(&this->anim_, DialCarousel::anim_exec_cb_);
  lv_anim_set_ready_cb(&this->anim_, DialCarousel::anim_ready_cb_);
  lv_anim_start(&this->anim_);
  return true;
}

void DialCarousel::anim_exec_cb_(void *var, int32_t progress) {
  auto *self = static_cast<DialCarousel *>(var);
  self->apply_animation_frame(progress, self->anim_direction_, self->anim_pill_shift_max_);
}

void DialCarousel::anim_ready_cb_(lv_anim_t *anim) {
  auto *self = static_cast<DialCarousel *>(anim->var);
  if (self == nullptr)
    return;

  self->animating_ = false;
  if (self->center_sub_ != nullptr) {
    lv_obj_set_style_opa(self->center_sub_, 255, LV_PART_MAIN);
  }
  if (self->on_ready_ != nullptr) {
    self->on_ready_(self->on_ready_context_);
  }
  self->on_ready_ = nullptr;
  self->on_ready_context_ = nullptr;
}

}  // namespace dial_carousel
}  // namespace esphome
