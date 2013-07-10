#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0x91, 0x41, 0xB6, 0x28, 0xBC, 0x89, 0x49, 0x8E, 0xB1, 0x47, 0x70, 0x3C, 0xDF, 0x3D, 0xA3, 0x89 }

PBL_APP_INFO(MY_UUID, "Rescue Time", "Small Stone Apps", 1, 0, DEFAULT_MENU_ICON, APP_INFO_WATCH_FACE);

void load_bitmaps();
void unload_bitmaps();
void handle_init(AppContextRef ctx);
void handle_deinit(AppContextRef ctx);

void update_productivity(int percent);
void update_time();
void update_date();

Window window;
TextLayer layer_time;
TextLayer layer_date;
TextLayer layer_percent;
BitmapLayer layer_face;
InverterLayer layer_invert;
HeapBitmap bmp_face_happy;
HeapBitmap bmp_face_meh;
HeapBitmap bmp_face_sad;

static int productivity_level = 65;

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit
  };
  app_event_loop(params, &handlers);
}

void load_bitmaps() {
  heap_bitmap_init(&bmp_face_happy, RESOURCE_ID_FACE_HAPPY);
  heap_bitmap_init(&bmp_face_meh, RESOURCE_ID_FACE_MEH);
  heap_bitmap_init(&bmp_face_sad, RESOURCE_ID_FACE_SAD);
}

void unload_bitmaps() {
  heap_bitmap_deinit(&bmp_face_happy);
  heap_bitmap_deinit(&bmp_face_meh);
  heap_bitmap_deinit(&bmp_face_sad);
}

void handle_init(AppContextRef ctx) {

  resource_init_current_app(&APP_RESOURCES);
  load_bitmaps();

  window_init(&window, "Watchface");
  window_stack_push(&window, true);

  text_layer_init(&layer_percent, GRect(72, 110, 72, 40));
  text_layer_set_text_color(&layer_percent, GColorBlack);
  text_layer_set_background_color(&layer_percent, GColorClear);
  text_layer_set_font(&layer_percent, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  text_layer_set_text_alignment(&layer_percent, GTextAlignmentCenter);
  layer_add_child(&window.layer, &layer_percent.layer);

  text_layer_init(&layer_time, GRect(0, 20, 144, 50));
  text_layer_set_text_color(&layer_time, GColorBlack);
  text_layer_set_background_color(&layer_time, GColorClear);
  text_layer_set_font(&layer_time, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(&layer_time, GTextAlignmentCenter);
  layer_add_child(&window.layer, &layer_time.layer);

  text_layer_init(&layer_date, GRect(0, 60, 144, 40));
  text_layer_set_text_color(&layer_date, GColorBlack);
  text_layer_set_background_color(&layer_date, GColorClear);
  text_layer_set_font(&layer_date, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(&layer_date, GTextAlignmentCenter);
  layer_add_child(&window.layer, &layer_date.layer);

  bitmap_layer_init(&layer_face, GRect(0, 104, 72, 48));
  bitmap_layer_set_alignment(&layer_face, GAlignCenter);
  bitmap_layer_set_bitmap(&layer_face, &bmp_face_happy.bmp);
  layer_add_child(&window.layer, &layer_face.layer);

  inverter_layer_init(&layer_invert, window.layer.frame);
  layer_add_child(&window.layer, &layer_invert.layer);

  update_time();
  update_date();
  update_productivity(79);
}


void handle_deinit(AppContextRef ctx) {
  unload_bitmaps();
}

void update_productivity(int percent) {
  productivity_level = percent;

  if (percent >= 70) {
    bitmap_layer_set_bitmap(&layer_face, &bmp_face_happy.bmp);
  }
  else if (percent >= 60) {
    bitmap_layer_set_bitmap(&layer_face, &bmp_face_meh.bmp);
  }
  else {
    bitmap_layer_set_bitmap(&layer_face, &bmp_face_sad.bmp);
  }

  static char percent_str[] = "XXXXX";
  snprintf(percent_str, strlen(percent_str), "%02d%s", percent, "%");
  text_layer_set_text(&layer_percent, percent_str);

  GRect window_bounds = layer_get_bounds(window_get_root_layer(&window));
  int height = (int)((float)window_bounds.size.h * ((float)percent) / 100.0f);
  GRect percent_bounds;
  percent_bounds.origin = GPoint(0,window_bounds.size.h - height);
  percent_bounds.size = GSize(window_bounds.size.w, height);
  layer_set_bounds(&layer_invert.layer, percent_bounds);
}

void update_time() {
  PblTm now;
  get_time(&now);

  static char time_text[] = "00:00";
  string_format_time(time_text, sizeof(time_text), "%H:%M", &now);
  text_layer_set_text(&layer_time, time_text);
}

void update_date() {
  PblTm now;
  get_time(&now);

  static char date_text[] = "DD/MM/YYYY";
  string_format_time(date_text, sizeof(date_text), "%d/%m/%Y", &now);
  text_layer_set_text(&layer_date, date_text);
}