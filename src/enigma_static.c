#include <pebble.h>

static Window *window;

//Setup Text Layers.
static Layer *year_layer;
static Layer *time_layer;
static Layer *date_layer;
static TextLayer *top_random;
static TextLayer *bottom_random;

static char* toprand;
static char* botrand;

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

static char* format_string(char *string) {
  char *numbers = malloc(4*2+1);
  int n = 0;
  for (size_t i = 0; i < 4; i++) {
    numbers[n] = string[i];
    n++;
    numbers[n] = ' ';
    n++;
  }
  numbers[n] = '\0'; //terminate the string
  return numbers;
}

static void update_time(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, PBL_IF_COLOR_ELSE(GColorOrange, GColorLightGray));

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H%M" : "%I%M", tick_time);

  graphics_fill_rect(ctx, GRect(0, 0,bounds.size.w,bounds.size.h-1),0,GCornerNone);
  graphics_draw_text(ctx, format_string(s_buffer), fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
  			GRect(0, -7, bounds.size.w, bounds.size.h),
  			GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void update_date(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_text_color(ctx, GColorWhite);

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), "%m%d", tick_time);

  graphics_draw_text(ctx, format_string(s_buffer), fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
  			GRect(0, 0, bounds.size.w, 42),
  			GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void update_year(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_text_color(ctx, GColorWhite);

  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), "%Y", tick_time);

  graphics_draw_text(ctx, format_string(s_buffer), fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS),
  			GRect(0, 0, bounds.size.w, 42),
  			GTextOverflowModeFill, GTextAlignmentCenter, NULL);
}

static void window_load(Window *window) {
  //get a string of random numbers.
  toprand = malloc(8+1);
  botrand = malloc(8+1);
  snprintf(toprand, 8+1, "%d %d %d %d", getRandNumber(), getRandNumber(), getRandNumber(), getRandNumber());
  snprintf(botrand, 8+1, "%d %d %d %d", getRandNumber(), getRandNumber(), getRandNumber(), getRandNumber());

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(window, GColorBlack);

  //draw random numbers
  //these are static for the life of the run. Because fuck it. who cares.
  top_random = text_layer_create(GRect(0, -32, bounds.size.w, 42));
  text_layer_set_font(top_random, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(top_random, GTextAlignmentCenter);
  text_layer_set_text_color(top_random, PBL_IF_COLOR_ELSE(GColorDarkGray, GColorWhite));
  text_layer_set_background_color(top_random, GColorClear);
  text_layer_set_text(top_random, toprand);
  layer_add_child(window_layer, text_layer_get_layer(top_random));

  year_layer = layer_create(GRect(0, 12, bounds.size.w, 42));
  layer_add_child(window_layer, year_layer);
  layer_set_update_proc(year_layer, update_year);

  time_layer = layer_create(GRect(0, 63, bounds.size.w, 42));
  layer_add_child(window_layer,time_layer);
  layer_set_update_proc(time_layer, update_time);

  date_layer = layer_create(GRect(0, 100, bounds.size.w, 42));
  layer_add_child(window_layer, date_layer);
  layer_set_update_proc(date_layer, update_date);

  bottom_random = text_layer_create(GRect(0, 144, bounds.size.w, 42));
  text_layer_set_font(bottom_random, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  text_layer_set_text_alignment(bottom_random, GTextAlignmentCenter);
  text_layer_set_text_color(bottom_random, PBL_IF_COLOR_ELSE(GColorDarkGray, GColorWhite));
  text_layer_set_background_color(bottom_random, GColorClear);
  text_layer_set_text(bottom_random, botrand);
  layer_add_child(window_layer, text_layer_get_layer(bottom_random));
}

static void window_unload(Window *window) {
  text_layer_destroy(top_random);
  text_layer_destroy(bottom_random);
  layer_destroy(year_layer);
  layer_destroy(time_layer);
  layer_destroy(date_layer);
  toprand = NULL;
  botrand = NULL;
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  srand(time(NULL)); //seed random number generator
  //subscribe to multiple tick events to update layers accordingly.
  tick_timer_service_subscribe(MINUTE_UNIT | DAY_UNIT | YEAR_UNIT, tick_handler);
  window_stack_push(window, false);
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
