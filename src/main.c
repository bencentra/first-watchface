#include <pebble.h>

/*
*  My first watch face for Pebble Time.
*  Influenced by Kiezel's "Essential" and "Simply Time" watchfaces.
*/
  
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
  
static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer, *s_weather_layer;
static GFont *s_font_leco_lg, *s_font_leco_sm;
static char s_time_buffer[10], s_num_buffer[4], s_day_buffer[6], s_date_buffer[12], s_temp_buffer[8], s_cond_buffer[32], s_weather_buffer[32];

/*
*  Watchface
*/

static void update_time() {
  // Get time struct
  time_t tmp = time(NULL);
  struct tm *tick_time = localtime(&tmp);

  // Set the time
  if(clock_is_24h_style() == true) {
    strftime(s_time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  } 
  else {
    strftime(s_time_buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  // Display the time
  text_layer_set_text(s_time_layer, s_time_buffer);
}

static void update_date() {
  // Get time struct
  time_t tmp = time(NULL);
  struct tm *t = localtime(&tmp);
  
  // Set the date
  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
  snprintf(s_date_buffer, sizeof(s_date_buffer), "%s %s", s_day_buffer, s_num_buffer);
  
  // Display the date
  text_layer_set_text(s_date_layer, s_date_buffer);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  
  // Create fonts
  s_font_leco_lg = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_LECO_BOLD_LETTERS_42));
  s_font_leco_sm = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_LECO_BOLD_LETTERS_18));
  
  // Create the date layer
  s_date_layer = text_layer_create(GRect(0, 36, 144, 168));
  text_layer_set_text(s_date_layer, s_date_buffer);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_font(s_date_layer, s_font_leco_sm);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  
  // Create the time layer
  s_time_layer = text_layer_create(GRect(0, 54, 144, 168));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, s_font_leco_lg);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  // Destroy everything!
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  fonts_unload_custom_font(s_font_leco_lg);
  fonts_unload_custom_font(s_font_leco_sm);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
  update_date();
}

/*
*  AppMessage
*/

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  
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