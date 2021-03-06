#include <pebble.h>
#include "drawing.h"
#include "../main.h"

extern int *flag_colors[], num_stripes[];

static int hour, min;
static char center_time[] = "hh:mm";

int if_quickview_else(int if_no, int if_yes) {
  if(layer_get_unobstructed_bounds(window_get_root_layer(main_window)).size.h == layer_get_bounds(window_get_root_layer(main_window)).size.h) {
    return if_no;
  } else {
    return if_yes;
  }
}

void update_time() {
  //get a tm structure
  time_t temp = time(NULL);
  struct tm *t = localtime(&temp);

  strftime(center_time, sizeof(center_time), clock_is_24h_style() ? "%H:%M" : "%I:%M", t);

  hour = t->tm_hour % 12;
  min = t->tm_min;
}

static void draw_flag(int segments, int colors[], GContext *ctx) {
  GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(main_window));
  
  int h, w;

  switch(settings.rot_flag) {
    default:
      h = bounds.size.h / segments + (bounds.size.h % segments != 0);
      w = bounds.size.w;
      break;
    case 1:
      h = bounds.size.h;
      w = -1 * bounds.size.w / segments - (bounds.size.w % segments != 0);
      break;
    case 2:
      h = -1 * bounds.size.h / segments - (-1 * bounds.size.h % segments != 0);
      w = bounds.size.w;
      break;
    case 3: 
      h = bounds.size.h;
      w = bounds.size.w / segments + (bounds.size.w % segments != 0);
  }

  for(int i = 0; i < segments; i++) {
    GRect flag_stripe;

    switch(settings.rot_flag) {
      default:
        flag_stripe = GRect(0, h * i, w, h);
        break;
      case 1:
        flag_stripe = GRect(bounds.size.w + (w * i), 0, w, h);
        break;
      case 2:
        flag_stripe = GRect(0, bounds.size.h + (h * i), w, h);
        break;
      case 3:
        flag_stripe = GRect(w * i, 0, w, h);
        break;
    }

    graphics_context_set_fill_color(ctx, GColorFromHEX(colors[i]));
    graphics_fill_rect(ctx, flag_stripe, 0, GCornerNone);
  }
}

static void draw_dots_bg(GContext *ctx, int radius) {
  GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(main_window));
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, settings.bg_color2);
  graphics_fill_circle(ctx, center, radius + if_quickview_else(10, 6));

  graphics_context_set_fill_color(ctx, settings.main_color);

  for(int i = 0; i < settings.num_hour_dots; i++) {
    int angle = TRIG_MAX_ANGLE * i / settings.num_hour_dots;

    GPoint dot = {
      .x = center.x + sin_lookup(angle) * radius / TRIG_MAX_RATIO,
      .y = center.y - cos_lookup(angle) * radius / TRIG_MAX_RATIO
    };

    graphics_fill_circle(ctx, dot, settings.dot_size);
  }
}

static void draw_hour_dot(GContext *ctx, int radius) {
  GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(main_window));
  GPoint center = grect_center_point(&bounds);

  graphics_context_set_fill_color(ctx, settings.accent_color);

  int angle = TRIG_MAX_ANGLE * hour / 12;

  GPoint dot = {
    .x = center.x + sin_lookup(angle) * radius / TRIG_MAX_RATIO,
    .y = center.y - cos_lookup(angle) * radius / TRIG_MAX_RATIO
  };

  graphics_fill_circle(ctx, dot, settings.dot_size);
}

static void draw_min_hand(GContext *ctx, int length) {
  GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(main_window));
  GPoint center = grect_center_point(&bounds);

  int angle = TRIG_MAX_ANGLE * min / 60;

  GPoint end_point = {
    .x = center.x + sin_lookup(angle) * length / TRIG_MAX_RATIO,
    .y = center.y - cos_lookup(angle) * length / TRIG_MAX_RATIO
  };

  graphics_context_set_stroke_color(ctx, settings.accent_color);
  graphics_context_set_stroke_width(ctx, settings.min_hand_size);
  graphics_draw_line(ctx, center, end_point);

  graphics_context_set_fill_color(ctx, settings.bg_color2);
  graphics_fill_circle(ctx, center, 30);
}

static void draw_digital_time(GContext *ctx) {
  GRect bounds = layer_get_unobstructed_bounds(window_get_root_layer(main_window));

  int height = if_quickview_else(28, 24);

  GRect box = GRect(0, bounds.size.h / 2 - (height / 2 + 4), bounds.size.w, height);

  graphics_context_set_text_color(ctx, settings.accent_color);
  
  if(bounds.size.h == layer_get_bounds(window_get_root_layer(main_window)).size.h) {
    graphics_draw_text(ctx, center_time, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD), box, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
  } else {
    graphics_draw_text(ctx, center_time, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), box, GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, 0);
  }
}

void draw_flag_update_proc(Layer *layer, GContext *ctx) {
  draw_flag(num_stripes[settings.num_flag], flag_colors[settings.num_flag], ctx);
}

void draw_dots_bg_update_proc(Layer *layer, GContext *ctx) {
  int dist_from_center = PBL_IF_ROUND_ELSE(60, if_quickview_else(58,46));

  draw_dots_bg(ctx, dist_from_center);
}

void draw_analog_time_update_proc(Layer *layer, GContext *ctx) {
  int dist_from_center = PBL_IF_ROUND_ELSE(60, if_quickview_else(58, 46));

  draw_hour_dot(ctx, dist_from_center);
  draw_min_hand(ctx, dist_from_center);
}

void draw_digital_time_update_proc(Layer *layer, GContext *ctx) {
  draw_digital_time(ctx);
}