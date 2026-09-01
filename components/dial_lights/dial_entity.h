#pragma once

#include <cstring>

// A Home Assistant entity enables its Dial page when it is a real entity id.
// Leave the substitution empty or set it to "<domain>.disabled" to hide the page.
// Legacy package placeholders stay hidden for existing configs.
inline bool dial_ha_entity_enabled(const char *entity) {
  if (entity == nullptr || entity[0] == '\0')
    return false;

  const char *dot = strrchr(entity, '.');
  if (dot != nullptr && strcmp(dot + 1, "disabled") == 0)
    return false;

  if (strcmp(entity, "timer.your_timer") == 0)
    return false;
  if (strcmp(entity, "weather.your_location") == 0)
    return false;

  return true;
}

// 0 timer, 1 lights, 2 covers, 3 garage, 4 switches, 5 scenes, 6 ac, 7 music, 8 home
static constexpr int DIAL_MENU_MAX = 9;
static constexpr int DIAL_FUNC_HOME = 8;

struct DialMenuCounts {
  size_t lights{0};
  size_t covers{0};
  size_t garages{0};
  size_t switches{0};
  size_t scenes{0};
  size_t climates{0};
  size_t media{0};
};

inline int dial_collect_menu_items(int *items, const char *timer_entity, const DialMenuCounts &c) {
  int n = 0;
  if (dial_ha_entity_enabled(timer_entity))
    items[n++] = 0;
  if (c.lights > 0)
    items[n++] = 1;
  if (c.covers > 0)
    items[n++] = 2;
  if (c.garages > 0)
    items[n++] = 3;
  if (c.switches > 0)
    items[n++] = 4;
  if (c.scenes > 0)
    items[n++] = 5;
  if (c.climates > 0)
    items[n++] = 6;
  if (c.media > 0)
    items[n++] = 7;
  items[n++] = DIAL_FUNC_HOME;
  return n;
}
