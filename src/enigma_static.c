#include <pebble.h>

typedef struct __attribute__((__packed__)) {
  int32_t t_fg_color;
  int32_t t_bg_color;
  int32_t m_fg_color;
  int32_t m_bg_color;
  int32_t r_fg_color;
} settings;

struct app {
  Window *window;
  Layer *year_layer;
  Layer *time_layer;
  Layer *date_layer;
  Layer *top_random;
  Layer *bottom_random;
  GSize textSize;
  bool is_obstructed;
  settings config;
} *app;

#define FONT_PADDING 6
#define NUMBER_GAP 10
#ifdef PBL_ROUND
#define PADDING 17
#else
#define PADDING 0
#endif

static int getRandNumber() {
    return rand() % 9;
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if (units_changed & MINUTE_UNIT) {
    layer_mark_dirty(app->time_layer);
  }
  if (units_changed & DAY_UNIT) {
    layer_mark_dirty(app->date_layer);
  }
  if (units_changed & YEAR_UNIT) {
    layer_mark_dirty(app->year_layer);
  }
}

#ifdef PBL_ROUND
static void draw_round_numbers(GContext *ctx, GRect bounds, int height) {
  char tmp[2];
  snprintf(tmp, 2, "%d", getRandNumber());
  graphics_draw_text(ctx, tmp, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
        GRect(-app->textSize.w/2, height, app->textSize.w, bounds.size.h),
        GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  snprintf(tmp, 2, "%d", getRandNumber());
  graphics_draw_text(ctx, tmp, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
        GRect(bounds.size.w-app->textSize.w/2, height, app->textSize.w, bounds.size.h),
        GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}
#endif

static void update_time(Layer *layer, GContext *ctx) {
  // APP_LOG(APP_LOG_LEVEL_INFO, "Drawing Time Layer.");
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_text_color(ctx, GColorFromHEX(app->config.t_fg_color));
  graphics_context_set_fill_color(ctx, GColorFromHEX(app->config.t_bg_color));
  graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, bounds.size.h-1),0,GCornerNone);

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[5][5];
  strftime(s_buffer[0], sizeof(s_buffer[0]), clock_is_24h_style() ? "%H%M" : "%I%M", tick_time);

  int n = 0;
  for (size_t i = 1; i < 5; i++) {
    s_buffer[i][0] = s_buffer[0][n];
    s_buffer[i][1] = '\n';
    graphics_draw_text(ctx, s_buffer[i], fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
    			GRect((NUMBER_GAP * n) + (app->textSize.w * n) + FONT_PADDING + PADDING, -7, app->textSize.w, bounds.size.h),
    			GTextOverflowModeFill, GTextAlignmentCenter, NULL);
    n++;
  }

  #ifdef PBL_ROUND
  draw_round_numbers(ctx, bounds, -7);
  #endif
}

static void update_date(Layer *layer, GContext *ctx) {
  // APP_LOG(APP_LOG_LEVEL_INFO, "Drawing Date Layer.");
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_text_color(ctx, GColorFromHEX(app->config.m_fg_color));

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[5][5];
  strftime(s_buffer[0], sizeof(s_buffer), "%m%d", tick_time);

  int n = 0;
  for (size_t i = 1; i < 5; i++) {
    s_buffer[i][0] = s_buffer[0][n];
    s_buffer[i][1] = '\n';
    graphics_draw_text(ctx, s_buffer[i], fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
    			GRect((NUMBER_GAP * n) + (app->textSize.w * n) + FONT_PADDING + PADDING, 0, app->textSize.w, bounds.size.h),
    			GTextOverflowModeFill, GTextAlignmentCenter, NULL);
    n++;
  }

  #ifdef PBL_ROUND
  draw_round_numbers(ctx, bounds, 0);
  #endif
}

static void update_year(Layer *layer, GContext *ctx) {
  // APP_LOG(APP_LOG_LEVEL_INFO, "Drawing Year Layer.");
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_text_color(ctx, GColorFromHEX(app->config.m_fg_color));

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[5][5];
  strftime(s_buffer[0], sizeof(s_buffer[0]), "%Y", tick_time);
  int n = 0;
  for (size_t i = 1; i < 5; i++) {
    s_buffer[i][0] = s_buffer[0][n];
    s_buffer[i][1] = '\n';
    graphics_draw_text(ctx, s_buffer[i], fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
    			GRect((NUMBER_GAP * n) + (app->textSize.w * n) + FONT_PADDING  + PADDING, 0, app->textSize.w, bounds.size.h),
    			GTextOverflowModeFill, GTextAlignmentCenter, NULL);
    n++;
  }

  #ifdef PBL_ROUND
  draw_round_numbers(ctx, bounds, 0);
  #endif

}

static void generate_random(Layer *layer, GContext *ctx) {
  // APP_LOG(APP_LOG_LEVEL_INFO, "Drawing Random Number Layers.");
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_text_color(ctx, PBL_IF_COLOR_ELSE(
    GColorFromHEX(app->config.r_fg_color),
    GColorFromHEX(app->config.m_fg_color)));
  char tmp[2];
  for (size_t i = 0; i < 4; i++) {
    snprintf(tmp, 2, "%d", getRandNumber());
    graphics_draw_text(ctx, tmp, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
    			GRect((NUMBER_GAP * i) + (app->textSize.w * i) + FONT_PADDING + PADDING, 0, app->textSize.w, bounds.size.h),
    			GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  }
}

static void obstructed_will_change(GRect final_unobstructed_screen_area, void *context) {
  GRect year_frame = layer_get_frame(app->year_layer);
  GRect time_frame = layer_get_frame(app->time_layer);
  GRect date_frame = layer_get_frame(app->date_layer);
  GRect full_bounds = layer_get_bounds(window_get_root_layer(app->window));

  if (!grect_equal(&full_bounds, &final_unobstructed_screen_area)) {
    // Screen is about to become obstructed, hide the date
    layer_set_hidden(app->top_random, true);
    layer_set_hidden(app->bottom_random, true);

    year_frame.origin.y = -10;
    layer_set_frame(app->year_layer, year_frame);

    time_frame.origin.y = app->textSize.h-3;
    layer_set_frame(app->time_layer, time_frame);

    date_frame.origin.y = app->textSize.h*2-10;
    layer_set_frame(app->date_layer, date_frame);
  } else {
    year_frame.origin.y = 12;
    layer_set_frame(app->year_layer, year_frame);

    time_frame.origin.y = 63;
    layer_set_frame(app->time_layer, time_frame);

    date_frame.origin.y = 100;
    layer_set_frame(app->date_layer, date_frame);

    layer_set_hidden(app->top_random, false);
    layer_set_hidden(app->bottom_random, false);
  }
}

static void window_load(Window *window) {
  app->textSize = graphics_text_layout_get_content_size("0",
                fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
                GRect(0, 0, 50, 42), GTextOverflowModeFill,
                GTextAlignmentCenter);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(window, GColorFromHEX(app->config.m_bg_color));

  GRect unobstructed_bounds = layer_get_unobstructed_bounds(window_layer);
  app->is_obstructed = !grect_equal(&bounds, &unobstructed_bounds);

  UnobstructedAreaHandlers unobstructed_handlers = {
    .will_change = obstructed_will_change,
  };

  unobstructed_area_service_subscribe(unobstructed_handlers, NULL);

  //draw random numbers
  app->top_random = layer_create(GRect(0, PBL_IF_RECT_ELSE(-32, -26), bounds.size.w, app->textSize.h));
  layer_add_child(window_layer, app->top_random);
  layer_set_update_proc(app->top_random, generate_random);

  app->bottom_random = layer_create(GRect(0, PBL_IF_RECT_ELSE(144, 150), bounds.size.w, app->textSize.h));
  layer_add_child(window_layer, app->bottom_random);
  layer_set_update_proc(app->bottom_random, generate_random);

  app->year_layer = layer_create(GRect(0, PBL_IF_RECT_ELSE(app->is_obstructed ? -10 : 12, 18), bounds.size.w, app->textSize.h));
  layer_add_child(window_layer, app->year_layer);
  layer_set_update_proc(app->year_layer, update_year);

  app->time_layer = layer_create(GRect(0, PBL_IF_RECT_ELSE(app->is_obstructed ? app->textSize.h-3 : 63, 69), bounds.size.w, app->textSize.h));
  layer_add_child(window_layer, app->time_layer);
  layer_set_update_proc(app->time_layer, update_time);

  app->date_layer = layer_create(GRect(0, PBL_IF_RECT_ELSE(app->is_obstructed ? app->textSize.h*2-10 : 100, 106), bounds.size.w, app->textSize.h));
  layer_add_child(window_layer, app->date_layer);
  layer_set_update_proc(app->date_layer, update_date);

  if (app->is_obstructed) {
    layer_set_hidden(app->top_random, true);
    layer_set_hidden(app->bottom_random, true);
  }
}

static void window_unload(Window *window) {
  layer_destroy(app->top_random);
  layer_destroy(app->bottom_random);
  layer_destroy(app->year_layer);
  layer_destroy(app->time_layer);
  layer_destroy(app->date_layer);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  if (!dict_find(iter, MESSAGE_KEY_t_bg)) return;
  app->config.t_bg_color = dict_find(iter, MESSAGE_KEY_t_bg)->value->int32;
  app->config.t_fg_color = dict_find(iter, MESSAGE_KEY_t_fg)->value->int32;
  app->config.m_bg_color = dict_find(iter, MESSAGE_KEY_m_bg)->value->int32;
  app->config.m_fg_color = dict_find(iter, MESSAGE_KEY_m_fg)->value->int32;
  #ifdef PBL_COLOR
  app->config.r_fg_color = dict_find(iter, MESSAGE_KEY_r_fg)->value->int32;
  #endif
  window_set_background_color(app->window, GColorFromHEX(app->config.m_bg_color));
  layer_mark_dirty(app->time_layer);
}

static void init(void) {
  app = calloc(1, sizeof(*app));

  app->window = window_create();
  app->config.t_fg_color = 16777215;
  app->config.t_bg_color = 16733695;
  app->config.m_bg_color = 0;
  app->config.m_fg_color = 16777215;
  app->config.r_fg_color = 5592405;

  if (persist_exists(MESSAGE_KEY_config)) {
    persist_read_data(MESSAGE_KEY_config, &app->config, sizeof(app->config));
  }

  window_set_window_handlers(app->window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  srand(time(NULL)); //seed random number generator
  //subscribe to multiple tick events to update layers accordingly.
  tick_timer_service_subscribe(MINUTE_UNIT | DAY_UNIT | YEAR_UNIT, tick_handler);
  window_stack_push(app->window, false);

  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(128, 0);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  persist_write_data(MESSAGE_KEY_config, &app->config, sizeof(app->config));
  if (app) free(app);
  window_destroy(app->window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
