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
  if (strcmp(entity, "climate.your_ac") == 0)
    return false;
  if (strcmp(entity, "media_player.your_player") == 0)
    return false;
  if (strcmp(entity, "weather.your_location") == 0)
    return false;

  return true;
}
