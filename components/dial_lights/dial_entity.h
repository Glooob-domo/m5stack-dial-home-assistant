#pragma once

// 0 alarm, 1 lights, 2 covers, 3 garage, 4 switches, 5 scenes, 6 ac, 7 music, 8 home, 9 temps
static constexpr int DIAL_MENU_MAX = 10;
static constexpr int DIAL_FUNC_HOME = 8;
static constexpr int DIAL_FUNC_TEMPS = 9;

struct DialMenuCounts {
  size_t lights{0};
  size_t covers{0};
  size_t garages{0};
  size_t switches{0};
  size_t scenes{0};
  size_t climates{0};
  size_t media{0};
  size_t temperatures{0};
};

inline int dial_collect_menu_items(int *items, const DialMenuCounts &c) {
  int n = 0;
  // Alarm is always available: purely local, no Home Assistant entity needed.
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
  if (c.temperatures > 0)
    items[n++] = DIAL_FUNC_TEMPS;
  if (c.climates > 0)
    items[n++] = 6;
  if (c.media > 0)
    items[n++] = 7;
  items[n++] = DIAL_FUNC_HOME;
  return n;
}
