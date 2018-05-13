#include "pebble.h"

static Window *s_main_window;

// This is a scroll layer
static ScrollLayer *s_scroll_layer;

// We also use a text layer to scroll in the scroll layer
static TextLayer *s_text_layer;

// Lorum ipsum to have something to scroll
static char s_scroll_text[] = "Riscaldamento, bpm 50% MFC :\r\n\r\n    500 m (200 m crawl completo \n 200 m dorso completo \n 100 m rana completa).\r\n\r\nA2 fase aerobica, bpm 65% MFC:\r\n\r\n    4 x (1 x 25 m, riposo 10 sec. + 1 x 50 m , riposo 20 sec. + 1 x 25 m, riposo 10 sec.) una serie per stile: delfino, dorso rana, crawl. Nessun riposo tra una serie e l\'altra (400 m misto frazionato).\r\n    8 x 200 m, crawl completo. Riposo 20 sec. tra uno scatto e l\'altro.\r\n    8 x 50 m, gambe crawl con tavoletta. Riposo 20 sec. tra uno scatto e l\'altro.\r\n    8 x 25 m, crawl completo. Riposo 30 sec. tra uno scatto e l\'altro. Tutti sostenuti.\r\n\r\nDefaticamento:\r\n\r\n    100 m crawl completo.\r\n\r\n Tot. 3200 m";

// Setup the scroll layer on window load
// We do this here in order to be able to get the max used text size
static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  GRect max_text_bounds = GRect(0, 0, bounds.size.w, 2000);

  // Initialize the scroll layer
  s_scroll_layer = scroll_layer_create(bounds);

  // This binds the scroll layer to the window so that up and down map to scrolling
  // You may use scroll_layer_set_callbacks to add or override interactivity
  scroll_layer_set_click_config_onto_window(s_scroll_layer, window);

  // Scrolls through text on a round screen by page (scroll offset is changed by frame's height)
#ifdef PBL_ROUND
  scroll_layer_set_paging(s_scroll_layer, true);
#endif

  // Initialize the text layer
  s_text_layer = text_layer_create(max_text_bounds);
  text_layer_set_text(s_text_layer, s_scroll_text);

  // Change the font to a nice readable one
  // This is system font; you can inspect pebble_fonts.h for all system fonts
  // or you can take a look at feature_custom_font to add your own font
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  // Add the layers for display
  scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_text_layer));

  layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));

  // Fit the text to the round screen with inset around edge of screen
#ifdef PBL_ROUND
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  uint8_t inset = 4;
  text_layer_enable_screen_text_flow_and_paging(s_text_layer, inset);
#endif

  // Trim text layer and scroll content to fit text box
  GSize max_size = text_layer_get_content_size(s_text_layer);
  text_layer_set_size(s_text_layer, max_size);
  scroll_layer_set_content_size(s_scroll_layer, GSize(bounds.size.w, max_size.h + 4));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
  scroll_layer_destroy(s_scroll_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}