#include <pebble.h>

/*
*  My first watch face for Pebble Time.
*  Influenced by Kiezel's "Essential" and "Simply Time" watchfaces.
*/
  
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1
  
static Window *s_main_window;
static TextLayer *s_time_label, *s_day_label, *s_num_label;
static Layer *s_date_layer, *s_time_layer;
static GFont *s_font_leco_lg, *s_font_leco_sm;
static char s_time_buffer[10], s_num_buffer[4], s_day_buffer[6];
  
/*
*  Watchface
*/

// static void update_time() {
//   // Get time struct
//   time_t tmp = time(NULL);
//   struct tm *tick_time = localtime(&tmp);
  
//   // Create long-lived buffer
//   static char buffer[] = "00:00";
  
//   // Set the time
//   if(clock_is_24h_style() == true) {
//     strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
//   } 
//   else {
//     strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
//   }
  
//   // Display the time
//   text_layer_set_text(s_time_layer, buffer);
// }

static void date_update_proc(Layer *layer, GContext *ctx) {
  // Get time struct
  time_t tmp = time(NULL);
  struct tm *t = localtime(&tmp);
  
  // Set the date
  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
  text_layer_set_text(s_day_label, s_day_buffer);
  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
  text_layer_set_text(s_num_label, s_num_buffer);
}

static void time_update_proc(Layer *layer, GContext *ctx) {
//   update_time();
  // Get time struct
  time_t tmp = time(NULL);
  struct tm *tick_time = localtime(&tmp);
  
  if(clock_is_24h_style() == true) {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M", tick_time);
  }
  else {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%I:%M", tick_time);
  }
  text_layer_set_text(s_time_label, s_time_buffer);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Create fonts
  s_font_leco_lg = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_LECO_BOLD_LETTERS_42));
  s_font_leco_sm = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KEY_LECO_BOLD_LETTERS_18));
  
  // Create day label
  s_day_label = text_layer_create(GRect(0, 42, 70, 42));
  text_layer_set_text(s_day_label, s_day_buffer);
  text_layer_set_background_color(s_day_label, GColorClear);
  text_layer_set_text_color(s_day_label, GColorBlack);
  text_layer_set_font(s_day_label, s_font_leco_sm);
  text_layer_set_text_alignment(s_day_label, GTextAlignmentRight);
  
  // Create num label
  s_num_label = text_layer_create(GRect(74, 42, 70, 42));
  text_layer_set_text(s_num_label, s_num_buffer);
  text_layer_set_background_color(s_num_label, GColorClear);
  text_layer_set_text_color(s_num_label, GColorBlack);
  text_layer_set_font(s_num_label, s_font_leco_sm);
  text_layer_set_text_alignment(s_num_label, GTextAlignmentLeft);
  
  // Create the date layer
  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);
  layer_add_child(s_date_layer, text_layer_get_layer(s_day_label));
  layer_add_child(s_date_layer, text_layer_get_layer(s_num_label));
  
  // Create the time label
  s_time_label = text_layer_create(GRect(0, 54, 144, 168));
  text_layer_set_background_color(s_time_label, GColorClear);
  text_layer_set_text_color(s_time_label, GColorBlack);
  text_layer_set_font(s_time_label, s_font_leco_lg);
  text_layer_set_text_alignment(s_time_label, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_label));
  
  // Create the time layer
  s_time_layer = layer_create(bounds);
  layer_set_update_proc(s_time_layer, time_update_proc);
  layer_add_child(window_layer, s_time_layer);
}

static void main_window_unload(Window *window) {
  // Destroy everything!
//   text_layer_destroy(s_time_layer);
  layer_destroy(s_time_layer);
  layer_destroy(s_date_layer);
  text_layer_destroy(s_day_label);
  text_layer_destroy(s_num_label);
  text_layer_destroy(s_time_label);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_main_window));
//   update_time();
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
//   update_time();
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