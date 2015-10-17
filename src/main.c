#include <pebble.h>
#include <ctype.h>

/*
*  My first watch face for Pebble Time.
*  Influenced by Kiezel's "Essential" and "Simply Time" watchfaces.
*/
  
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
#define KEY_OUTER_COLOR 2
#define KEY_INNER_COLOR 3
#define KEY_TIME_FORMAT 4
  
static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer, *s_weather_layer;
static Layer *s_circle_layer, *s_rect_layer;
static GFont *s_font_leco_lg, *s_font_leco_sm;
static bool twenty_four_hour_format = false;
static int outer, inner;
static GColor outer_color, inner_color, inner_text_color;

/*
*  Watchface
*/

static void update_time() {
  // Get time struct
  time_t tmp = time(NULL);
  struct tm *tick_time = localtime(&tmp);

  static char s_time_buffer[] = "00:00";

  // Set the time
  if (twenty_four_hour_format == true) {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M", tick_time);
  } 
  else {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%I:%M", tick_time);
  }
  
  // Display the time
  text_layer_set_text(s_time_layer, s_time_buffer);
}

static void update_date() {
  // Get time struct
  time_t tmp = time(NULL);
  struct tm *t = localtime(&tmp);

  static char s_day_buffer[] = "SUN";
  static char s_num_buffer[] = "01";
  static char s_date_buffer[] = "SUN 01";
  
  // Set the date
  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
  snprintf(s_date_buffer, sizeof(s_date_buffer), "%s %s", s_day_buffer, s_num_buffer);
  
  // Display the date
  text_layer_set_text(s_date_layer, s_date_buffer);
}

static void draw_circles(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  const int16_t half_w = bounds.size.w / 2;
  const int16_t half_h = bounds.size.h / 2;
  
  // Draw the outer circle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(half_w, half_h), half_w - 4);
  
  // Draw the inner circle
  graphics_context_set_fill_color(ctx, inner_color);
  graphics_fill_circle(ctx, GPoint(half_w, half_h), half_w - 6);
}

static void draw_rectangles(Layer *layer, GContext *ctx) {
  int stroke = 4;
  
  // Draw the outer rectangle
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0, 60, 144, 51), 0, GCornerNone);
  
  // Draw the inner rectangle
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(0, 60 + stroke, 144, 51 - (stroke * 2)), 0, GCornerNone);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create fonts
  s_font_leco_lg = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_LECO_BOLD_LETTERS_42));
  s_font_leco_sm = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_LECO_BOLD_LETTERS_16));

  // Check for persisted settings
  if (persist_read_int(KEY_OUTER_COLOR)) {
    outer = persist_read_int(KEY_OUTER_COLOR);
    outer_color = GColorFromHEX(outer);
  }
  else {
    outer_color = GColorBlack;
  }
  if (persist_read_int(KEY_INNER_COLOR)) {
    inner  = persist_read_int(KEY_INNER_COLOR);
    inner_color = GColorFromHEX(inner);
  }
  else {
    inner_color = GColorWhite;
  }
  if (persist_read_bool(KEY_TIME_FORMAT)) {
    twenty_four_hour_format = persist_read_bool(KEY_TIME_FORMAT);
  }

  // Color the background
  window_set_background_color(window, outer_color);
  
  // Draw the circles
  s_circle_layer = layer_create(bounds);
  layer_set_update_proc(s_circle_layer, draw_circles);
  
  // Create the date layer
  s_date_layer = text_layer_create(GRect(0, 37, 144, 168));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_font(s_date_layer, s_font_leco_sm);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  
  // Create the weather layer
  s_weather_layer = text_layer_create(GRect(0, 113, 144, 168));
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorBlack);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "Loading");
  text_layer_set_font(s_weather_layer, s_font_leco_sm);
  
  // Draw the rectangle
  s_rect_layer = layer_create(bounds);
  layer_set_update_proc(s_rect_layer, draw_rectangles);
  
  // Create the time layer
  s_time_layer = text_layer_create(GRect(0, 58, 144, 168));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, s_font_leco_lg);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Assemble the watchface
  layer_add_child(window_layer, s_circle_layer);
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));
  layer_add_child(window_layer, s_rect_layer);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  // Destroy everything!
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_weather_layer);
  layer_destroy(s_circle_layer);
  layer_destroy(s_rect_layer);
  fonts_unload_custom_font(s_font_leco_lg);
  fonts_unload_custom_font(s_font_leco_sm);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  update_date();
  // Get weather update every 30 minutes
  if(tick_time->tm_min % 30 == 0) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    dict_write_uint8(iter, 0, 0);
    app_message_outbox_send();
  }
}

/*
*  AppMessage
*/

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Store incoming information
  static char s_temp_buffer[8];
  static char s_cond_buffer[32];
  static char s_weather_buffer[32];
  
  // Read first item
  Tuple *t = dict_read_first(iterator);
  
  // For all items
  while(t != NULL) {
    switch(t->key) {
      case KEY_TEMPERATURE:
        snprintf(s_temp_buffer, sizeof(s_temp_buffer), "%d°", (int)t->value->int32);
        break;
      case KEY_CONDITIONS:
        snprintf(s_cond_buffer, sizeof(s_cond_buffer), "%s", t->value->cstring);
        break;
      case KEY_OUTER_COLOR:
        outer = t->value->int32;
        persist_write_int(KEY_OUTER_COLOR, outer);
        window_set_background_color(s_main_window, GColorFromHEX(outer));
        break;
      case KEY_INNER_COLOR: 
        inner = t->value->int32;
        persist_write_int(KEY_INNER_COLOR, inner);
        inner_color = GColorFromHEX(inner);
        layer_mark_dirty(window_get_root_layer(s_main_window));
        break;
      case KEY_TIME_FORMAT:
        twenty_four_hour_format = t->value->int8;
        persist_write_int(KEY_TIME_FORMAT, twenty_four_hour_format);
        update_time();
        break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "KEY %d not recognized!", (int)t->key);
        break;
    }
    
    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Assemble the full string and display
  snprintf(s_weather_buffer, sizeof(s_weather_buffer), "%s %s", s_temp_buffer, s_cond_buffer);
  text_layer_set_text(s_weather_layer, s_weather_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

/*
*  App
*/

static void init() {
  // Create window and set lifecycle callbacks
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  // Regiseter AppMessage callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  // Display initial time
  update_time();
  update_date();
}

static void deinit() {
  tick_timer_service_unsubscribe();
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
