#include <pebble.h>

#define STATUS_KEY 0
#define MESSAGE_KEY 1

#define NUM_MENU_SECTIONS 2
#define NUM_FIRST_MENU_ITEMS 3
#define NUM_SECOND_MENU_ITEMS 1

static Window *s_main_window;
static Window *windows_squeezebox;
static Window *window_advance_squeezebox;
static TextLayer *text_layer;
static TextLayer *s_time_layer;
static TextLayer *s_weather_layer;

static GBitmap *s_background_bitmap;

static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem s_first_menu_items[NUM_FIRST_MENU_ITEMS];
static SimpleMenuItem s_second_menu_items[NUM_SECOND_MENU_ITEMS];
static GBitmap *s_menu_icon_image;

static ActionBarLayer *action_bar;

static GFont s_weather_font;

static bool s_special_flag = false;
static int s_hit_count = 0;

// Advance SqueezeBox Menu
static SimpleMenuLayer *s_advance_squeezebox_menu_layer;
static SimpleMenuSection s_advance_squeezebox_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem s_advance_squeezebox_first_menu_items[NUM_FIRST_MENU_ITEMS];
static SimpleMenuItem s_advance_squeezebox_second_menu_items[NUM_SECOND_MENU_ITEMS];


void send_int(char* key, char* cmd)
{
  DictionaryIterator* dictionaryIterator = NULL;
  app_message_outbox_begin (&dictionaryIterator);
  dict_write_cstring (dictionaryIterator, STATUS_KEY, key);
  dict_write_cstring (dictionaryIterator, MESSAGE_KEY, cmd);
  dict_write_end (dictionaryIterator);
  app_message_outbox_send();
}

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "click SELECT");
  text_layer_set_text(s_time_layer, "play");
    send_int("sb", "play");
}
void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "LOoooooong SELECT");
  window_stack_push(window_advance_squeezebox, true);
}
void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "LOoooooong SELECT release");
}

void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "click UP");
  text_layer_set_text(s_time_layer, "previous");
    send_int("sb", "play");
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "click DOWN");
  text_layer_set_text(s_time_layer, "next");
    send_int("sb", "dddd");
}

static void menu_select_callback(int index, void *ctx) {
  s_first_menu_items[index].subtitle = "You've hit select here!";
  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}

void click_config_provider2(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_click_handler);
  window_long_click_subscribe(BUTTON_ID_SELECT, 700,(ClickHandler) select_long_click_handler, select_long_click_release_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_click_handler);
}

static void menu_squeezebox(int index, void *ctx) {
  window_stack_push(windows_squeezebox, true);
}

static void windows_squeezebox_load(Window *window){
  // layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));

  // Initialize the action bar:
  action_bar = action_bar_layer_create();
  action_bar_layer_set_background_color(action_bar, GColorRed);
  // Associate the action bar with the window:
  action_bar_layer_add_to_window(action_bar, window);
  // Set the click config provider:
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider2);

  // Set the icons:
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_PLAY);
  // The loading of the icons is omitted for brevity... See gbitmap_create_with_resource()
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_SELECT, s_background_bitmap, true);
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_PREVIOUS);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, s_background_bitmap, true);
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_NEXT);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, s_background_bitmap, true);
}
static void windows_squeezebox_unload(Window *window){
  action_bar_layer_destroy(action_bar);
}

static void menu_advance_squeezebox(int index, void *ctx) {
  window_stack_push(window_advance_squeezebox, true);
}

static void windows_advance_squeezebox_load(Window *window) {
  // Although we already defined NUM_FIRST_MENU_ITEMS, you can define
  // an int as such to easily change the order of menu items later
  int num_a_items = 0;

  s_advance_squeezebox_first_menu_items[num_a_items++] = (SimpleMenuItem) {
    .title = "one",
    .callback = menu_select_callback,
  };
  s_advance_squeezebox_first_menu_items[num_a_items++] = (SimpleMenuItem) {
    .title = "two",
    .subtitle = "Here's a subtitle",
    .callback = menu_select_callback,
  };
  s_advance_squeezebox_first_menu_items[num_a_items++] = (SimpleMenuItem) {
    .title = "Third Item",
    .subtitle = "This has an icon",
    .callback = menu_select_callback,
  };

  s_advance_squeezebox_second_menu_items[0] = (SimpleMenuItem) {
    .title = "Special Item",
  };

  s_advance_squeezebox_menu_sections[0] = (SimpleMenuSection) {
    .num_items = NUM_FIRST_MENU_ITEMS,
    .items = s_advance_squeezebox_first_menu_items,
  };
  s_advance_squeezebox_menu_sections[1] = (SimpleMenuSection) {
    .title = "Yet Another Section",
    .num_items = NUM_SECOND_MENU_ITEMS,
    .items = s_advance_squeezebox_second_menu_items,
  };

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_advance_squeezebox_menu_layer = simple_menu_layer_create(bounds, window, s_advance_squeezebox_menu_sections, NUM_MENU_SECTIONS, NULL);

  layer_add_child(window_layer, simple_menu_layer_get_layer(s_advance_squeezebox_menu_layer));
}

static void windows_advance_squeezebox_unload(Window *window) {
  simple_menu_layer_destroy(s_advance_squeezebox_menu_layer);
}

static void special_select_callback(int index, void *ctx) {
  // Of course, you can do more complicated things in a menu item select callback
  // Here, we have a simple toggle
  s_special_flag = !s_special_flag;

  SimpleMenuItem *menu_item = &s_second_menu_items[index];

  if (s_special_flag) {
    menu_item->subtitle = "Okay, it's not so special.";
  } else {
    menu_item->subtitle = "Well, maybe a little.";
  }

  if (++s_hit_count > 5) {
    menu_item->title = "Very Special Item";
  }

  layer_mark_dirty(simple_menu_layer_get_layer(s_simple_menu_layer));
}

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}


static void main_window_load(Window *window) {
  // s_menu_icon_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MENU_ICON_1);

  // Although we already defined NUM_FIRST_MENU_ITEMS, you can define
  // an int as such to easily change the order of menu items later
  int num_a_items = 0;

  s_first_menu_items[num_a_items++] = (SimpleMenuItem) {
    .title = "SqueezeBox",
    .callback = menu_squeezebox,
  };
  s_first_menu_items[num_a_items++] = (SimpleMenuItem) {
    .title = "kodi",
    .subtitle = "Here's a subtitle",
    .callback = menu_select_callback,
  };
  s_first_menu_items[num_a_items++] = (SimpleMenuItem) {
    .title = "Third Item",
    .subtitle = "This has an icon",
    .callback = menu_advance_squeezebox,
    .icon = s_menu_icon_image,
  };

  s_second_menu_items[0] = (SimpleMenuItem) {
    .title = "Special Item",
    .callback = special_select_callback,
  };

  s_menu_sections[0] = (SimpleMenuSection) {
    .num_items = NUM_FIRST_MENU_ITEMS,
    .items = s_first_menu_items,
  };
  s_menu_sections[1] = (SimpleMenuSection) {
    .title = "Yet Another Section",
    .num_items = NUM_SECOND_MENU_ITEMS,
    .items = s_second_menu_items,
  };

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_simple_menu_layer = simple_menu_layer_create(bounds, window, s_menu_sections, NUM_MENU_SECTIONS, NULL);

  layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));

  // Create time TextLayer
  s_time_layer = text_layer_create(GRect(0, 130, 144, 25));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorRed);
  text_layer_set_text(s_time_layer, "00:00");

  s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
  text_layer_set_font(s_time_layer, s_weather_font);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);

  // Destroy weather elements
  text_layer_destroy(s_weather_layer);
  fonts_unload_custom_font(s_weather_font);

  simple_menu_layer_destroy(s_simple_menu_layer);
  gbitmap_destroy(s_menu_icon_image);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {

}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  // APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  windows_squeezebox = window_create();
  window_advance_squeezebox = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  window_set_window_handlers(windows_squeezebox, (WindowHandlers) {
    .load = windows_squeezebox_load,
    .unload = windows_squeezebox_unload
  });

  window_set_window_handlers(window_advance_squeezebox, (WindowHandlers) {
    .load = windows_advance_squeezebox_load,
    .unload = windows_advance_squeezebox_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

}

static void deinit() {
    // Destroy Window
    window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}