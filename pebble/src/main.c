#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_output_layer;

SmartstrapAttribute *attribute;

static int s_button_presses;
static bool s_service_available;

static void update_ui();

/******************************** Smartstraps *********************************/

static void strap_availability_handler(SmartstrapServiceId service_id, bool is_available) {
  // A service's availability has changed
  APP_LOG(APP_LOG_LEVEL_INFO, "Service %d is %s available", (int)service_id, is_available ? "now" : "NOT");

  // Remember if the raw service is available
  s_service_available = (is_available && service_id == SMARTSTRAP_RAW_DATA_SERVICE_ID);
  update_ui();
}

static void strap_notify_handler(SmartstrapAttribute *attribute) {
  // Button was pressed!
  s_button_presses++;
  update_ui();
  vibes_short_pulse();
}

static void strap_init() {
  attribute = smartstrap_attribute_create(SMARTSTRAP_RAW_DATA_SERVICE_ID, SMARTSTRAP_RAW_DATA_ATTRIBUTE_ID, 64);

  // Subscribe to smartstrap events
  smartstrap_subscribe((SmartstrapHandlers) {
    .availability_did_change = strap_availability_handler,
    .notified = strap_notify_handler
  });
}

/************************************* UI *************************************/

static void update_ui() {
  static char s_buffer[128];
  snprintf(s_buffer, sizeof(s_buffer), "Connected: %s\n\nButton presses:\n%d",
    (s_service_available ? "YES" : "NO"), s_button_presses);
  text_layer_set_text(s_output_layer, s_buffer);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_output_layer = text_layer_create(GRect(bounds.origin.x, PBL_IF_ROUND_ELSE(20, bounds.origin.y), bounds.size.w, bounds.size.h));
  text_layer_set_font(s_output_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(s_output_layer, GTextAlignmentCenter);
  text_layer_set_text_color(s_output_layer, GColorWhite);
  text_layer_set_background_color(s_output_layer, GColorClear);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_output_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_output_layer);
  window_destroy(s_main_window);
}

/************************************ App *************************************/

static void init() {
  strap_init();

  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);

  update_ui();
}

static void deinit() { }

int main() {
  init();
  app_event_loop();
  deinit();
}
