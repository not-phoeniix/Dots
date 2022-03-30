#include <pebble.h>
#include "cfg.h"
#include "../main.h"

static void default_settings() {
    settings.main_color = GColorWhite;
    settings.accent_color = GColorRed;
    settings.bg_color = GColorBlack;
    settings.num_hour_dots = 12;
    settings.dot_size = 2;
    settings.min_hand_size = 2;
}

void load_settings() {
    default_settings();
    persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

void save_settings() {
    persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}