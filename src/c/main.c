/*
*Pi-Light Controller
*Author: James Poole
*Date: 3/12/16
*
*/

#include <pebble.h>

static Window *window;
static TextLayer *red_text_layer;
static TextLayer *blue_text_layer;
static TextLayer *green_text_layer;
static int s_arduino_pin[10];
static int red_state = 0;
static int blue_state = 0;
static int green_state = 0;

typedef enum {
    OffKey = 0,
    OnKey,
} LightKey;

static void light_background_color() {
    if (red_state == 0 && blue_state == 0 && green_state == 0) {
        window_set_background_color(window, GColorLightGray);
    } else if (red_state == 0 && blue_state == 0 && green_state == 1) {
        window_set_background_color(window, GColorGreen);
    } else if (red_state == 0 && blue_state == 1 && green_state == 0) {
        window_set_background_color(window, GColorBlue);
    } else if (red_state == 0 && blue_state == 1 && green_state == 1) {
        window_set_background_color(window, GColorCyan);
    } else if (red_state == 1 && blue_state == 0 && green_state == 0) {
        window_set_background_color(window, GColorRed);
    } else if (red_state == 1 && blue_state == 0 && green_state == 1) {
        window_set_background_color(window, GColorYellow);
    } else if (red_state == 1 && blue_state == 1 && green_state == 0) {
        window_set_background_color(window, GColorPurple);
    } else if (red_state == 1 && blue_state == 1 && green_state == 1) {
        window_set_background_color(window, GColorOrange);
    }
}

static bool send_to_phone(int light_key, int s_arduino_pin) {
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    
    dict_write_uint8(iter, light_key, s_arduino_pin);

    dict_write_end(iter);
    app_message_outbox_send();
    return true;
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
 // text_layer_set_text(text_layer, "Up");
    if (red_state == 0) {
        send_to_phone(1, 18);
        red_state = 1;
        text_layer_set_text(red_text_layer, "RED - ON");
    } else {
        send_to_phone(0, 18);
        red_state = 0;
        text_layer_set_text(red_text_layer, "RED - OFF");
    }
    light_background_color();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (blue_state == 0) {
        send_to_phone(1, 23);
        blue_state = 1;
        text_layer_set_text(blue_text_layer, "BLUE - ON");
    } else {
        send_to_phone(0, 23);
        blue_state = 0;
        text_layer_set_text(blue_text_layer, "BLUE - OFF");
    }
    light_background_color();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    //text_layer_set_text(text_layer, "Down");
      if (green_state == 0) {
          send_to_phone(1, 24);
          green_state = 1;
          text_layer_set_text(green_text_layer, "GREEN - ON");
      } else {
          send_to_phone(0, 24);
          green_state = 0;
          text_layer_set_text(green_text_layer, "GREEN - OFF");
      }
      light_background_color();
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Failed to Send!");
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
  
    window_set_background_color(window, GColorLightGray);
    
    red_text_layer = text_layer_create(GRect(0, 15, bounds.size.w, 20));
    text_layer_set_text(red_text_layer, "RED - OFF");
    text_layer_set_text_color(red_text_layer, GColorRed);
    text_layer_set_text_alignment(red_text_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(red_text_layer));

    green_text_layer = text_layer_create(GRect(0, 75, bounds.size.w, 20));
    text_layer_set_text(green_text_layer, "GREEN - OFF");
    text_layer_set_text_color(green_text_layer, GColorGreen);
    text_layer_set_text_alignment(green_text_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(green_text_layer));
    
    blue_text_layer = text_layer_create(GRect(0, 135, bounds.size.w, 20));
    text_layer_set_text(blue_text_layer, "BLUE - OFF");
    text_layer_set_text_color(blue_text_layer, GColorBlue);
    text_layer_set_text_alignment(blue_text_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(blue_text_layer));
}

static void window_unload(Window *window) {
    text_layer_destroy(red_text_layer);
    text_layer_destroy(blue_text_layer);
    text_layer_destroy(green_text_layer);
}

static void init(void) {
    app_message_register_outbox_failed(out_failed_handler);
    
    app_message_open(64, 64);

    window = window_create();
    window_set_click_config_provider(window, click_config_provider);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    const bool animated = true;
    window_stack_push(window, animated);
}

static void deinit(void) {
    window_destroy(window);
}

int main(void) {
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

    app_event_loop();
    deinit();
}