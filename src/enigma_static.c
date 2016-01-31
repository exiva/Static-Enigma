#include <pebble.h>

static Window *window;

//Setup Layers.
static Layer *year_layer;
static Layer *time_layer;
static Layer *date_layer;
static Layer *top_random;
static Layer *bottom_random;

static GSize textSize;

enum {
  bg = 0,
  fg = 1
};

static int32_t fg_color;
static int32_t bg_color;

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
    layer_mark_dirty(time_layer);
  }
  if (units_changed & DAY_UNIT) {
    layer_mark_dirty(date_layer);
  }
  if (units_changed & YEAR_UNIT) {
    layer_mark_dirty(year_layer);
  }
}

#ifdef PBL_ROUND
static void draw_round_numbers(GContext *ctx, GRect bounds, int height) {
  char tmp[2];
  snprintf(tmp, 2, "%d", getRandNumber());
  graphics_draw_text(ctx, tmp, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
        GRect(-textSize.w/2, height, textSize.w, bounds.size.h),
        GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  snprintf(tmp, 2, "%d", getRandNumber());
  graphics_draw_text(ctx, tmp, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
        GRect(bounds.size.w-textSize.w/2, height, textSize.w, bounds.size.h),
        GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}
#endif

static void update_time(Layer *layer, GContext *ctx) {
  // APP_LOG(APP_LOG_LEVEL_INFO, "Drawing Time Layer.");
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_text_color(ctx, PBL_IF_COLOR_ELSE(GColorFromHEX(fg_color), GColorWhite));
  graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(GColorFromHEX(bg_color), GColorLightGray));
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
    			GRect((NUMBER_GAP * n) + (textSize.w * n) + FONT_PADDING + PADDING, -7, textSize.w, bounds.size.h),
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
  graphics_context_set_text_color(ctx, GColorWhite);

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[5][5];
  strftime(s_buffer[0], sizeof(s_buffer), "%m%d", tick_time);

  int n = 0;
  for (size_t i = 1; i < 5; i++) {
    s_buffer[i][0] = s_buffer[0][n];
    s_buffer[i][1] = '\n';
    graphics_draw_text(ctx, s_buffer[i], fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
    			GRect((NUMBER_GAP * n) + (textSize.w * n) + FONT_PADDING + PADDING, 0, textSize.w, bounds.size.h),
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
  graphics_context_set_text_color(ctx, GColorWhite);

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[5][5];
  strftime(s_buffer[0], sizeof(s_buffer[0]), "%Y", tick_time);
  int n = 0;
  for (size_t i = 1; i < 5; i++) {
    s_buffer[i][0] = s_buffer[0][n];
    s_buffer[i][1] = '\n';
    graphics_draw_text(ctx, s_buffer[i], fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
    			GRect((NUMBER_GAP * n) + (textSize.w * n) + FONT_PADDING  + PADDING, 0, textSize.w, bounds.size.h),
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
  graphics_context_set_text_color(ctx, PBL_IF_COLOR_ELSE(GColorDarkGray, GColorWhite));
  char tmp[2];
  for (size_t i = 0; i < 4; i++) {
    snprintf(tmp, 2, "%d", getRandNumber());
    graphics_draw_text(ctx, tmp, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
    			GRect((NUMBER_GAP * i) + (textSize.w * i) + FONT_PADDING + PADDING, 0, textSize.w, bounds.size.h),
    			GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  }
}

static void window_load(Window *window) {
  textSize = graphics_text_layout_get_content_size("0",
                fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
                GRect(0, 0, 50, 42), GTextOverflowModeFill,
                GTextAlignmentCenter);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(window, GColorBlack);

  //draw random numbers
  top_random = layer_create(GRect(0, PBL_IF_RECT_ELSE(-32, -26), bounds.size.w, textSize.h));
  layer_add_child(window_layer, top_random);
  layer_set_update_proc(top_random, generate_random);

  bottom_random = layer_create(GRect(0, PBL_IF_RECT_ELSE(144, 150), bounds.size.w, textSize.h));
  layer_add_child(window_layer, bottom_random);
  layer_set_update_proc(bottom_random, generate_random);

  year_layer = layer_create(GRect(0, PBL_IF_RECT_ELSE(12, 18), bounds.size.w, textSize.h));
  layer_add_child(window_layer, year_layer);
  layer_set_update_proc(year_layer, update_year);

  time_layer = layer_create(GRect(0, PBL_IF_RECT_ELSE(63, 69), bounds.size.w, textSize.h));
  layer_add_child(window_layer,time_layer);
  layer_set_update_proc(time_layer, update_time);

  date_layer = layer_create(GRect(0, PBL_IF_RECT_ELSE(100, 106), bounds.size.w, textSize.h));
  layer_add_child(window_layer, date_layer);
  layer_set_update_proc(date_layer, update_date);
}

static void window_unload(Window *window) {
  layer_destroy(top_random);
  layer_destroy(bottom_random);
  layer_destroy(year_layer);
  layer_destroy(time_layer);
  layer_destroy(date_layer);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  if (!dict_find(iter, bg)) return;
  bg_color = dict_find(iter, bg)->value->int32;
  fg_color = dict_find(iter, fg)->value->int32;
  persist_write_int(bg, bg_color);
  persist_write_int(fg, fg_color);
  layer_mark_dirty(time_layer);
}

static void init(void) {
  window = window_create();
  if (persist_exists(bg)) {
    bg_color = persist_read_int(bg);
    fg_color = persist_read_int(fg);
  } else {
    bg_color = 16755200;
    fg_color = 16777215;
  }

  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  srand(time(NULL)); //seed random number generator
  //subscribe to multiple tick events to update layers accordingly.
  tick_timer_service_subscribe(MINUTE_UNIT | DAY_UNIT | YEAR_UNIT, tick_handler);
  window_stack_push(window, false);

  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(10, 0);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
