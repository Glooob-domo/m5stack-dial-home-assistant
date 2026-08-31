#pragma once

#include <cstdio>
#include <string>

#include "esphome/core/component.h"

#ifndef DIAL_UI_LANG
#define DIAL_UI_LANG 0
#endif

namespace esphome {
namespace dial_i18n {

#define DIAL_I18N_PICK(en, fr, es, de, it) \
  (DIAL_UI_LANG == 1   ? (fr) \
   : DIAL_UI_LANG == 2 ? (es) \
   : DIAL_UI_LANG == 3 ? (de) \
   : DIAL_UI_LANG == 4 ? (it) \
                       : (en))

inline const char *temp() { return DIAL_I18N_PICK("TEMP", "TEMP", "TEMP", "TEMP", "TEMP"); }
inline const char *hum() { return DIAL_I18N_PICK("HUM", "HUM", "HUM", "FEU", "UMID"); }
inline const char *aqi() { return DIAL_I18N_PICK("AQI", "IQA", "ICA", "AQI", "IQA"); }
inline const char *pressure() {
  return DIAL_I18N_PICK("PRESSURE", "PRESSION", "PRESION", "DRUCK", "PRESSIONE");
}
inline const char *wind() { return DIAL_I18N_PICK("WIND", "VENT", "VIENTO", "WIND", "VENTO"); }

inline const char *dow_sun() { return DIAL_I18N_PICK("Sun", "Dim", "Dom", "So", "Dom"); }
inline const char *dow_mon() { return DIAL_I18N_PICK("Mon", "Lun", "Lun", "Mo", "Lun"); }
inline const char *dow_tue() { return DIAL_I18N_PICK("Tue", "Mar", "Mar", "Di", "Mar"); }
inline const char *dow_wed() { return DIAL_I18N_PICK("Wed", "Mer", "Mie", "Mi", "Mer"); }
inline const char *dow_thu() { return DIAL_I18N_PICK("Thu", "Jeu", "Jue", "Do", "Gio"); }
inline const char *dow_fri() { return DIAL_I18N_PICK("Fri", "Ven", "Vie", "Fr", "Ven"); }
inline const char *dow_sat() { return DIAL_I18N_PICK("Sat", "Sam", "Sab", "Sa", "Sab"); }

inline const char *connecting() {
  return DIAL_I18N_PICK("Connecting...", "Connexion...", "Conectando...", "Verbinden...", "Connessione...");
}
inline const char *syncing() { return DIAL_I18N_PICK("Syncing...", "Synchro...", "Sincronizando...", "Sync...", "Sincronia..."); }
inline const char *connected() {
  return DIAL_I18N_PICK("Connected!", "Connecté !", "Conectado!", "Verbunden!", "Connesso!");
}
inline const char *home_assistant() { return "Home Assistant"; }

inline const char *menu_timer() { return DIAL_I18N_PICK("Timer", "Minuteur", "Temporiz.", "Timer", "Timer"); }
inline const char *menu_lights() { return DIAL_I18N_PICK("Lights", "Lumières", "Luces", "Licht", "Luci"); }
inline const char *menu_ac() { return DIAL_I18N_PICK("AC", "Clim", "Aire", "Klima", "Clima"); }
inline const char *menu_music() { return DIAL_I18N_PICK("Music", "Musique", "Musica", "Musik", "Musica"); }
inline const char *menu_home() { return DIAL_I18N_PICK("Home", "Accueil", "Inicio", "Start", "Home"); }
inline const char *menu_clock() { return DIAL_I18N_PICK("Clock", "Horloge", "Reloj", "Uhr", "Orologio"); }

inline const char *unavailable() {
  return DIAL_I18N_PICK("Unavailable", "Indispo.", "No disp.", "Nicht da", "Non disp.");
}
inline const char *no_timer() { return DIAL_I18N_PICK("No timer", "Inactif", "Sin timer", "Kein Timer", "Nessun timer"); }
inline const char *finished() { return DIAL_I18N_PICK("Finished", "Terminé", "Terminado", "Fertig", "Finito"); }
inline const char *timer_left() { return DIAL_I18N_PICK("%s left", "%s rest.", "%s rest.", "%s rest.", "%s rest."); }
inline const char *timer_paused() { return DIAL_I18N_PICK("Paused · %s", "Pause · %s", "Pausa · %s", "Pause · %s", "Pausa · %s"); }

inline const char *light_off() { return DIAL_I18N_PICK("Off", "Off", "Off", "Aus", "Off"); }
inline const char *light_on() { return DIAL_I18N_PICK("On", "On", "On", "An", "On"); }
inline const char *light_on_pct() { return DIAL_I18N_PICK("On · %d%%", "On · %d%%", "On · %d%%", "An · %d%%", "On · %d%%"); }
inline const char *lights_all_off() { return DIAL_I18N_PICK("All off", "Toutes off", "Todas off", "Alle aus", "Tutte off"); }
inline const char *lights_all_on() { return DIAL_I18N_PICK("All on", "Toutes on", "Todas on", "Alle an", "Tutte on"); }
inline const char *lights_n_on() { return DIAL_I18N_PICK("%d of %d on", "%d/%d on", "%d/%d on", "%d/%d an", "%d/%d on"); }

inline const char *hvac_cooling() { return DIAL_I18N_PICK("Cooling", "Froid", "Frio", "Kuehlen", "Freddo"); }
inline const char *hvac_heating() { return DIAL_I18N_PICK("Heating", "Chaud", "Calor", "Heizen", "Caldo"); }
inline const char *hvac_fan() { return DIAL_I18N_PICK("Fan", "Ventil.", "Vent.", "Luefter", "Vent."); }
inline const char *hvac_dry() { return DIAL_I18N_PICK("Dry", "Sec", "Seco", "Trocken", "Secco"); }
inline const char *hvac_auto() { return DIAL_I18N_PICK("Auto", "Auto", "Auto", "Auto", "Auto"); }
inline const char *hvac_off() { return DIAL_I18N_PICK("Off", "Off", "Off", "Aus", "Off"); }

inline const char *media_off() { return DIAL_I18N_PICK("Off", "Off", "Off", "Aus", "Off"); }
inline const char *media_idle() { return DIAL_I18N_PICK("No playback", "À l'arrêt", "Sin audio", "Kein Titel", "Fermo"); }
inline const char *media_paused() { return DIAL_I18N_PICK("Paused", "Pause", "Pausa", "Pause", "Pausa"); }
inline const char *media_playing() { return DIAL_I18N_PICK("Playing", "Lecture", "Reprod.", "Wiedergabe", "In play"); }

inline const char *ready_to_play() {
  return DIAL_I18N_PICK("Ready to Play", "Prêt à jouer", "Listo", "Bereit", "Pronto");
}

inline const char *light_title() { return DIAL_I18N_PICK("LIGHT", "LUMIERE", "LUZ", "LICHT", "LUCE"); }
inline const char *color() { return DIAL_I18N_PICK("COLOR", "COULEUR", "COLOR", "FARBE", "COLORE"); }
inline const char *white() { return DIAL_I18N_PICK("WHITE", "BLANC", "BLANCO", "WEISS", "BIANCO"); }
inline const char *on() { return DIAL_I18N_PICK("ON", "ON", "ON", "AN", "ON"); }
inline const char *off() { return DIAL_I18N_PICK("OFF", "OFF", "OFF", "AUS", "OFF"); }

inline const char *unit_hr() { return DIAL_I18N_PICK("HR", "H", "H", "STD", "H"); }
inline const char *unit_min() { return DIAL_I18N_PICK("MIN", "MIN", "MIN", "MIN", "MIN"); }
inline const char *unit_sec() { return DIAL_I18N_PICK("SEC", "SEC", "SEG", "SEK", "SEC"); }
inline const char *rst() { return DIAL_I18N_PICK("RST", "RAZ", "RST", "RST", "RST"); }
inline const char *cancel() { return DIAL_I18N_PICK("CANCEL", "ANNUL.", "ANULAR", "ABBR.", "ANNULLA"); }

inline const char *mode_auto() { return DIAL_I18N_PICK("AUTO", "AUTO", "AUTO", "AUTO", "AUTO"); }
inline const char *mode_cool() { return DIAL_I18N_PICK("COOL", "FROID", "FRIO", "KUHL", "FREDDO"); }
inline const char *mode_heat() { return DIAL_I18N_PICK("HEAT", "CHAUD", "CALOR", "HEIZ", "CALDO"); }
inline const char *mode_fan_only() { return DIAL_I18N_PICK("FAN ONLY", "VENTIL", "VENTIL", "LUEFTER", "VENTIL"); }
inline const char *mode_dry() { return DIAL_I18N_PICK("DRY", "SEC", "SECO", "TROCKEN", "SECCO"); }
inline const char *mode_heat_cool() { return DIAL_I18N_PICK("HEAT COOL", "MIXTE", "AUTO", "HEIZ/KUHL", "MIX"); }
inline const char *mode_off() { return DIAL_I18N_PICK("OFF", "OFF", "OFF", "AUS", "OFF"); }
inline const char *fan_fmt() { return DIAL_I18N_PICK("FAN %s", "VENT %s", "VENT %s", "LUEFT %s", "VENT %s"); }
inline const char *fan_unknown() { return DIAL_I18N_PICK("FAN ---", "VENT ---", "VENT ---", "LUEFT ---", "VENT ---"); }
inline const char *air_info() {
  return DIAL_I18N_PICK("Temp %.0f\xc2\xb0  Humid %.0f%%", "Temp %.0f\xc2\xb0  Hum %.0f%%",
                        "Temp %.0f\xc2\xb0  Hum %.0f%%", "Temp %.0f\xc2\xb0  Feu %.0f%%",
                        "Temp %.0f\xc2\xb0  Um %.0f%%");
}

inline const char *weekday(int day_of_week) {
  switch (day_of_week) {
    case 1:
      return dow_sun();
    case 2:
      return dow_mon();
    case 3:
      return dow_tue();
    case 4:
      return dow_wed();
    case 5:
      return dow_thu();
    case 6:
      return dow_fri();
    case 7:
      return dow_sat();
    default:
      return "---";
  }
}

inline const char *hvac_short(const std::string &mode) {
  if (mode == "auto")
    return mode_auto();
  if (mode == "cool")
    return mode_cool();
  if (mode == "heat")
    return mode_heat();
  if (mode == "fan_only")
    return mode_fan_only();
  if (mode == "dry")
    return mode_dry();
  if (mode == "heat_cool")
    return mode_heat_cool();
  if (mode == "off")
    return mode_off();
  return nullptr;
}

inline bool hvac_menu(const std::string &mode, char *buf, size_t len) {
  const char *label = nullptr;
  if (mode == "cool")
    label = hvac_cooling();
  else if (mode == "heat")
    label = hvac_heating();
  else if (mode == "fan_only")
    label = hvac_fan();
  else if (mode == "dry")
    label = hvac_dry();
  else if (mode == "auto" || mode == "heat_cool")
    label = hvac_auto();
  else if (mode == "off")
    label = hvac_off();
  if (label == nullptr)
    return false;
  snprintf(buf, len, "%s", label);
  return true;
}

#undef DIAL_I18N_PICK

class DialI18n : public Component {};

}  // namespace dial_i18n
}  // namespace esphome
