#include <pebble.h>

///////////////
// Variables //
///////////////
#define STATUS_KEY 0
#define MESSAGE_KEY 1
#define PLAT 2
#define TITLE_INFO 3
#define ARTIST_INFO 4
#define ALBUM_INFO 5
#define TAP_NOT_DATA true

#define NUM_MENU_SECTIONS 2
#define NUM_FIRST_MENU_ITEMS 2
#define NUM_SECOND_MENU_ITEMS 1

static Window *s_main_window;
static Window *windows_squeezebox;
static Window *window_advance_squeezebox;
static Window *windows_kodi;
static TextLayer *text_layer;
static TextLayer *s_title_info_layer;
static TextLayer *s_artist_info_layer;
static TextLayer *s_album_info_layer;
static TextLayer *s_weather_layer;

static GBitmap *s_background_bitmap;

static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem s_first_menu_items[NUM_FIRST_MENU_ITEMS];
static SimpleMenuItem s_second_menu_items[NUM_SECOND_MENU_ITEMS];
static GBitmap *s_menu_squeezebox_icon_image;
static GBitmap *s_menu_kodi_icon_image;

static ActionBarLayer *action_bar;

static GFont s_weather_font;

static bool s_special_flag = false;
static int s_hit_count = 0;

// Advance SqueezeBox Menu
static SimpleMenuLayer *s_advance_squeezebox_menu_layer;
static SimpleMenuSection s_advance_squeezebox_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem s_advance_squeezebox_first_menu_items[NUM_FIRST_MENU_ITEMS];
static SimpleMenuItem s_advance_squeezebox_second_menu_items[NUM_SECOND_MENU_ITEMS];

void action_bar_nav_v_kodi(Window *window);

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
  send_int("sb", "previous");
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "click DOWN");
  send_int("sb", "next");
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

  // Create title TextLayer
  s_title_info_layer = text_layer_create(GRect(0, 30, 144, 25));
  text_layer_set_background_color(s_title_info_layer, GColorClear);
  text_layer_set_text_color(s_title_info_layer, GColorRed);
  text_layer_set_text(s_title_info_layer, "title");
  // s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
  // text_layer_set_font(s_title_info_layer, s_weather_font);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_title_info_layer));

  // Create album TextLayer
  s_album_info_layer = text_layer_create(GRect(0, 60, 144, 25));
  text_layer_set_background_color(s_album_info_layer, GColorClear);
  text_layer_set_text_color(s_album_info_layer, GColorRed);
  text_layer_set_text(s_album_info_layer, "album");
  // s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
  // text_layer_set_font(s_album_info_layer, s_weather_font);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_album_info_layer));

  // Create artist TextLayer
  s_artist_info_layer = text_layer_create(GRect(0, 90, 144, 25));
  text_layer_set_background_color(s_artist_info_layer, GColorClear);
  text_layer_set_text_color(s_artist_info_layer, GColorRed);
  text_layer_set_text(s_artist_info_layer, "artist");
  // s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
  // text_layer_set_font(s_artist_info_layer, s_weather_font);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_artist_info_layer));

  //Request track info
  send_int("sb", "track_info");
}
static void windows_squeezebox_unload(Window *window){
  action_bar_layer_destroy(action_bar);
  text_layer_destroy(s_title_info_layer);
  text_layer_destroy(s_album_info_layer);
  text_layer_destroy(s_artist_info_layer);
}

void select_click_handler_kodi_movie(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "click kodi SELECT");
  send_int("kodi", "play");
}
// void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "LOoooooong SELECT");
//   window_stack_push(window_advance_squeezebox, true);
// }
// void select_long_click_release_handler(ClickRecognizerRef recognizer, void *context) {
//   APP_LOG(APP_LOG_LEVEL_DEBUG, "LOoooooong SELECT release");
// }

void up_click_handler_kodi_movie(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "click UP");
  send_int("kodi", "move_left");
}

void down_click_handler_kodi_movie(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "click DOWN");
  send_int("kodi", "move_right");
}

void click_config_provider_kodi_movie(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_click_handler_kodi_movie);
  // window_long_click_subscribe(BUTTON_ID_SELECT, 700,(ClickHandler) select_long_click_handler, select_long_click_release_handler);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_click_handler_kodi_movie);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down_click_handler_kodi_movie);
}

static void menu_kodi(int index, void *ctx) {
  window_stack_push(windows_kodi, true);
}

static void action_bar_movie_kodi(Window *window){
  // Initialize the action bar:
  action_bar = action_bar_layer_create();
  action_bar_layer_set_background_color(action_bar, GColorRed);
  // Associate the action bar with the window:
  action_bar_layer_add_to_window(action_bar, window);
  // Set the click config provider:
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider_kodi_movie);

  // Set the icons:
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_PLAY);
  // The loading of the icons is omitted for brevity... See gbitmap_create_with_resource()
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_SELECT, s_background_bitmap, true);
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_PREVIOUS);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, s_background_bitmap, true);
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_NEXT);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, s_background_bitmap, true);
}

static void data_handler(AccelData *data, uint32_t num_samples) {
  // Long lived buffer
  static char s_buffer[128];

  // Compose string of all data
  snprintf(s_buffer, sizeof(s_buffer), 
    "N X,Y,Z\n0 %d,%d,%d\n1 %d,%d,%d\n2 %d,%d,%d", 
    data[0].x, data[0].y, data[0].z, 
    data[1].x, data[1].y, data[1].z, 
    data[2].x, data[2].y, data[2].z
  );

  //Show the data
  text_layer_set_text(s_title_info_layer, s_buffer);
}

void select_click_handler_kodi_nav_h(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "click SELECT");
  send_int("kodi", "select");
}

void up_click_handler_kodi_nav_h(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "click UP");
  send_int("kodi", "left");
}

void down_click_handler_kodi_nav_h(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "click DOWN");
  send_int("kodi", "right");
}

static void tap_handler_kodi_nav_h(AccelAxisType axis, int32_t direction) {
  switch (axis) {
  case ACCEL_AXIS_X:
    if (direction > 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "X axis positive.");
    } else {
      APP_LOG(APP_LOG_LEVEL_INFO, "X axis negative.");
    }
    break;
  case ACCEL_AXIS_Y:
    if (direction > 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Y axis positive.");
      action_bar_nav_v_kodi(windows_kodi);
    } else {
      APP_LOG(APP_LOG_LEVEL_INFO, "Y axis negative.");
      action_bar_nav_v_kodi(windows_kodi);
    }
    break;
  case ACCEL_AXIS_Z:
    if (direction > 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Z axis positive.");
    } else {
      APP_LOG(APP_LOG_LEVEL_INFO, "Z axis negative.");
    }
    break;
  }
}

void click_config_provider_kodi_nav_h(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_click_handler_kodi_nav_h);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_click_handler_kodi_nav_h);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down_click_handler_kodi_nav_h);
}

static void action_bar_nav_h_kodi(Window *window){
  // Initialize the action bar:
  action_bar = action_bar_layer_create();
  action_bar_layer_set_background_color(action_bar, GColorBlue);
  // Associate the action bar with the window:
  action_bar_layer_add_to_window(action_bar, window);
  // Set the click config provider:
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider_kodi_nav_h);

  // Set the icons:
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_DOTH);
  // The loading of the icons is omitted for brevity... See gbitmap_create_with_resource()
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_SELECT, s_background_bitmap, true);
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_LEFT);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, s_background_bitmap, true);
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_RIGHT);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, s_background_bitmap, true);

  // Use tap service? If not, use data service
  if (TAP_NOT_DATA) {
    // Subscribe to the accelerometer tap service
    accel_tap_service_subscribe(tap_handler_kodi_nav_h);
  } else {
    // Subscribe to the accelerometer data service
    int num_samples = 3;
    accel_data_service_subscribe(num_samples, data_handler);

    // Choose update rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
  }
}

void select_click_handler_kodi_nav_v(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "click SELECT");
  send_int("kodi", "select");
}

void up_click_handler_kodi_nav_v(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "click UP");
  send_int("kodi", "up");
}

void down_click_handler_kodi_nav_v(ClickRecognizerRef recognizer, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "click DOWN");
  send_int("kodi", "down");
}

static void tap_handler_kodi_nav_v(AccelAxisType axis, int32_t direction) {
  switch (axis) {
  case ACCEL_AXIS_X:
    if (direction > 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "X axis positive.");
    } else {
      APP_LOG(APP_LOG_LEVEL_INFO, "X axis negative.");
    }
    break;
  case ACCEL_AXIS_Y:
    if (direction > 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Y axis positive.");
      action_bar_nav_h_kodi(windows_kodi);
    } else {
      APP_LOG(APP_LOG_LEVEL_INFO, "Y axis negative.");
      action_bar_nav_h_kodi(windows_kodi);
    }
    break;
  case ACCEL_AXIS_Z:
    if (direction > 0) {
      APP_LOG(APP_LOG_LEVEL_INFO, "Z axis positive.");
    } else {
      APP_LOG(APP_LOG_LEVEL_INFO, "Z axis negative.");
    }
    break;
  }
}

void click_config_provider_kodi_nav_v(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_click_handler_kodi_nav_v);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_click_handler_kodi_nav_v);
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down_click_handler_kodi_nav_v);
}

void action_bar_nav_v_kodi(Window *window){
  // Initialize the action bar:
  action_bar = action_bar_layer_create();
  action_bar_layer_set_background_color(action_bar, GColorGreen);
  // Associate the action bar with the window:
  action_bar_layer_add_to_window(action_bar, window);
  // Set the click config provider:
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider_kodi_nav_v);

  // Set the icons:
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_DOTV);
  // The loading of the icons is omitted for brevity... See gbitmap_create_with_resource()
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_SELECT, s_background_bitmap, true);
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_UP);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, s_background_bitmap, true);
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ICON_DOWN);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, s_background_bitmap, true);

  // Use tap service? If not, use data service
  if (TAP_NOT_DATA) {
    // Subscribe to the accelerometer tap service
    accel_tap_service_subscribe(tap_handler_kodi_nav_v);
  } else {
    // Subscribe to the accelerometer data service
    int num_samples = 3;
    accel_data_service_subscribe(num_samples, data_handler);

    // Choose update rate
    accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
  }
}

static void windows_kodi_load(Window *window){
  // Create title TextLayer
  s_title_info_layer = text_layer_create(GRect(0, 10, 144, 25));
  text_layer_set_background_color(s_title_info_layer, GColorClear);
  text_layer_set_text_color(s_title_info_layer, GColorRed);
  text_layer_set_text(s_title_info_layer, "Kodi");
  // s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
  // text_layer_set_font(s_title_info_layer, s_weather_font);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_title_info_layer));

  // Create Movie TextLayer
  s_album_info_layer = text_layer_create(GRect(0, 60, 144, 25));
  text_layer_set_background_color(s_album_info_layer, GColorClear);
  text_layer_set_text_color(s_album_info_layer, GColorRed);
  text_layer_set_text(s_album_info_layer, "Movie");
  // s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_PERFECT_DOS_20));
  // text_layer_set_font(s_album_info_layer, s_weather_font);
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_album_info_layer));

  //Load action bar
  action_bar_nav_h_kodi(window);

  //Request info
  send_int("kodi", "init");
}
static void windows_kodi_unload(Window *window){
  action_bar_layer_destroy(action_bar);
  text_layer_destroy(s_title_info_layer);
  text_layer_destroy(s_album_info_layer);

  if (TAP_NOT_DATA) {
    accel_tap_service_unsubscribe();
  } else {
    accel_data_service_unsubscribe();
  }
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
  };
  s_advance_squeezebox_first_menu_items[num_a_items++] = (SimpleMenuItem) {
    .title = "two",
    .subtitle = "Here's a subtitle",
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

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void layer_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, s_menu_squeezebox_icon_image, gbitmap_get_bounds(s_menu_squeezebox_icon_image));
}

static void main_window_load(Window *window) {
  s_menu_squeezebox_icon_image = gbitmap_create_with_resource(RESOURCE_ID_ICON_SQUEEZEBOX);
  s_menu_kodi_icon_image = gbitmap_create_with_resource(RESOURCE_ID_ICON_KODI);

  // Although we already defined NUM_FIRST_MENU_ITEMS, you can define
  // an int as such to easily change the order of menu items later
  int num_a_items = 0;

  s_first_menu_items[num_a_items++] = (SimpleMenuItem) {
    .title = "SqueezeBox",
    .callback = menu_squeezebox,
    .icon = s_menu_squeezebox_icon_image,
  };
  s_first_menu_items[num_a_items++] = (SimpleMenuItem) {
    .title = "Kodi",
    .callback = menu_kodi,
    .icon = s_menu_kodi_icon_image,
  };

  s_menu_sections[0] = (SimpleMenuSection) {
    .num_items = NUM_FIRST_MENU_ITEMS,
    .items = s_first_menu_items,
  };

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_simple_menu_layer = simple_menu_layer_create(bounds, window, s_menu_sections, NUM_MENU_SECTIONS, NULL);

  layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
}

static void main_window_unload(Window *window) {
  simple_menu_layer_destroy(s_simple_menu_layer);
  gbitmap_destroy(s_menu_squeezebox_icon_image);
  gbitmap_destroy(s_menu_kodi_icon_image);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  static char title_info_layer_buffer[32];
  static char artist_info_layer_buffer[32];
  static char album_info_layer_buffer[32];
  
  // APP_LOG(APP_LOG_LEVEL_ERROR, "inbox_received_callback");
  // Read first item
  Tuple *t = dict_read_first(iterator);
  while(t != NULL) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "111 %s", t->value->cstring);
    switch(t->key) {
      case PLAT:
        APP_LOG(APP_LOG_LEVEL_ERROR, "In PLAT");
        if (strcmp(t->value->cstring, "kodi") == 0) {
          APP_LOG(APP_LOG_LEVEL_ERROR, "In KODI");
          Tuple *t2 = dict_read_first(iterator);
          while(t2 != NULL) {
            switch(t2->key) {
               APP_LOG(APP_LOG_LEVEL_ERROR, "In KODI::: %s", t2->value->cstring);
               case TITLE_INFO:
                if(strcmp(t2->value->cstring, "NULL_NOT_PLAY") == 0) {
                  text_layer_set_text(s_album_info_layer, "Nothing");
                } else {
                  snprintf(album_info_layer_buffer, sizeof(album_info_layer_buffer), "%s", t2->value->cstring);
                  text_layer_set_text(s_album_info_layer, album_info_layer_buffer);
                  action_bar_movie_kodi(windows_kodi);
                }
                break;
              default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t2->key);
                break;
            }
            t2 = dict_read_next(iterator);
          }
        }else if (strcmp(t->value->cstring, "sq") == 0) {
          APP_LOG(APP_LOG_LEVEL_ERROR, "In SQ");
          Tuple *t2 = dict_read_first(iterator);
          while(t2 != NULL) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "In SQ::: %s", t2->value->cstring);
            switch(t2->key) {
              case TITLE_INFO:
                snprintf(title_info_layer_buffer, sizeof(title_info_layer_buffer), "%s", t2->value->cstring);
                text_layer_set_text(s_title_info_layer, title_info_layer_buffer);
                break;
              case ARTIST_INFO:
                snprintf(artist_info_layer_buffer, sizeof(artist_info_layer_buffer), "%s", t2->value->cstring);
                text_layer_set_text(s_album_info_layer, album_info_layer_buffer);
                break;
              case ALBUM_INFO:
                snprintf(album_info_layer_buffer, sizeof(album_info_layer_buffer), "%s", t2->value->cstring);
                text_layer_set_text(s_artist_info_layer, artist_info_layer_buffer);
                break;
              default:
                APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
                break;
            }
            t2 = dict_read_next(iterator);
          }
        }else {APP_LOG(APP_LOG_LEVEL_ERROR, "else");}
        break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key is not PLAT");
        break;
    }
    t = dict_read_next(iterator);
  }
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

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  windows_squeezebox = window_create();
  windows_kodi = window_create();
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

  window_set_window_handlers(windows_kodi, (WindowHandlers) {
    .load = windows_kodi_load,
    .unload = windows_kodi_unload
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