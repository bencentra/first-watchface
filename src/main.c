#include <pebble.h>

/*
*  My first watchface. 
*  Influenced by Kiezel's "Essential" and "Simply Time" watchfaces.
*/
  
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont *s_font_leco_lg, *s_font_leco_sm;
  
/*
*  Watchface
*/

static void main_window_load(Window *window) {
  // Create fonts
  s_font_leco_lg = fonts_get_system_font(FONT_KEY_LECO_38_BOLD_NUMBERS);
  s_font_leco_sm = fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS);
  
  // Create the time layer
  s_time_layer = text_layer_create(GRect(0, 0, 144, 168));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_font(s_time_layer, s_font_leco_lg);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  // Destroy everything!
  text_layer_destroy(s_time_layer);
}

static void update_time() {
  // Get time struct
  time_t tmp = time(NULL);
  struct tm *tick_time = localtime(&tmp);
  
  // Create long-lived buffer
  static char buffer[] = "00:00";
  
  // Set the time
  if(clock_is_24h_style() == true) {
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } 
  else {
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }
  
  // Display the time
  text_layer_set_text(s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
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
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}