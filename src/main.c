#include <pebble.h>
#include "analogue.h"

Window *window;
static Layer *layer, *s_hands_layer;
static TextLayer *s_time_layer, *s_date_layer, *s_param_layer;
static GColor foreground_color;
static GColor background_color;
static GPath *s_tick_paths[NUM_CLOCK_TICKS];
static GPath *s_minute_arrow, *s_hour_arrow;
static char s_date_buffer[10];
GPoint oldPoint;

int SmallRadius          = 0;
int LargeRadius          = 0;
const int WIDTH          = 144;
const int HEIGHT         = 168;
const int MAXREVOLUTIONS = 420;
const int MAXSPIROS      = 2;


static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time    = localtime(&temp);
  static char buffer[]    = "00:00";
 
  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) { strftime(buffer, sizeof("00:00"), "%H:%M", tick_time); } 
  else { strftime(buffer, sizeof("00:00"), "%I:%M", tick_time); }

  // Display time on the TextLayer
  //text_layer_set_text(s_time_layer, buffer);
  // Display date on DateLayer
  strftime(s_date_buffer, sizeof(s_date_buffer), "%a %d", tick_time);
  text_layer_set_text(s_date_layer, s_date_buffer);
}


static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  //int16_t second_hand_length = bounds.size.w / 2;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  //int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;

  // minute/hour hand
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  gpath_rotate_to(s_hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, s_hour_arrow);
  gpath_draw_outline(ctx, s_hour_arrow);

  gpath_rotate_to(s_minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, s_minute_arrow);
  gpath_draw_outline(ctx, s_minute_arrow);
  
  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 4);
}


static void update_layer(struct Layer *layer, GContext *ctx) {
    
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  int h,x,y,s,u,z = 0;
  time_t temp = time(NULL); 
  int32_t first_angle, second_angle, third_angle = 0;
  int16_t second_hand_length = rand() % 75;
  int randomizer = -1;
  //struct tm *tick_time    = localtime(&temp);
  //static char s_param_buff[] = "r=100 R=100 h=100";
  srand((unsigned) time(&temp));
  
  SmallRadius = 10 + rand() % 50;
  LargeRadius = 20 + rand() % 75;
  h           = 10 + rand() % 30;


  for (int j=1 ; j<MAXSPIROS ; j++) {
    s=0;
    #ifdef PBL_COLOR
      graphics_context_set_stroke_color(ctx,GColorFromRGB(rand()%256, rand()%256, rand()%256));
      graphics_context_set_stroke_width(ctx,1);
    #else
     graphics_context_set_stroke_color(ctx, GColorWhite);
    #endif
    while (s < MAXREVOLUTIONS) {
       // Run Pebble's own cos/sin lookup, quite faster
       //first_angle  = TRIG_MAX_ANGLE * s / 60;
       second_angle = TRIG_MAX_ANGLE * s / 60;
       third_angle  = TRIG_MAX_ANGLE * s /60 * randomizer * LargeRadius/SmallRadius;
       x = (int16_t)(sin_lookup(second_angle) * (int32_t)LargeRadius / TRIG_MAX_RATIO) + center.x;
       y = (int16_t)(-cos_lookup(second_angle) * (int32_t)LargeRadius / TRIG_MAX_RATIO) + center.y;
       z = x + (int16_t)(sin_lookup(third_angle) * (int32_t)(SmallRadius-h) / TRIG_MAX_RATIO);
       u = y +  (int16_t)(-cos_lookup(third_angle) * (int32_t)(SmallRadius-h) / TRIG_MAX_RATIO);
      
      if (s != 0) { graphics_draw_line(ctx, GPoint(z,u), oldPoint); }
      else { graphics_draw_pixel(ctx, GPoint(z,u)); }

      oldPoint = GPoint(z,u);
      s += 1;
    }
    
    #ifdef PBL_COLOR
      graphics_context_set_stroke_color(ctx,GColorFromRGB(rand()%256, rand()%256, rand()%256));
      graphics_context_set_stroke_width(ctx,2);
    #else
     graphics_context_set_stroke_color(ctx, GColorWhite);
    #endif
    
    SmallRadius = 10 + rand() % 50;
    LargeRadius = 20 + rand() % 75;
    s=0;
    while (s < MAXREVOLUTIONS) {
       // Run Pebble's own cos/sin lookup, quite faster
       //first_angle  = TRIG_MAX_ANGLE * s / 60;
       second_angle = TRIG_MAX_ANGLE * s / 60;
       third_angle  = TRIG_MAX_ANGLE * s /60 * randomizer * LargeRadius/SmallRadius;
       x = (int16_t)(sin_lookup(second_angle) * (int32_t)LargeRadius / TRIG_MAX_RATIO) + center.x;
       y = (int16_t)(-cos_lookup(second_angle) * (int32_t)LargeRadius / TRIG_MAX_RATIO) + center.y;
       z = x + (int16_t)(sin_lookup(third_angle) * (int32_t)(SmallRadius-h) / TRIG_MAX_RATIO);
       u = y +  (int16_t)(-cos_lookup(third_angle) * (int32_t)(SmallRadius-h) / TRIG_MAX_RATIO);
      
      if (s != 0) { graphics_draw_line(ctx, GPoint(z,u), oldPoint); }
      else { graphics_draw_pixel(ctx, GPoint(z,u)); }

      oldPoint = GPoint(z,u);
      s += 1;
    }
  } // for j
    
  //snprintf(s_param_buff, sizeof(s_param_buff), "r=%i R=%i h=%i", SmallRadius, LargeRadius, h);
  //text_layer_set_text(s_param_layer, s_param_buff);
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}


static void tap_handler(AccelAxisType axis, int32_t direction) {
  layer_mark_dirty(window_get_root_layer(window));
}


static void window_load(Window *window) {
  foreground_color = GColorWhite;
  background_color = GColorBlack;
  
  Layer *window_layer = window_get_root_layer(window);
  window_set_background_color(window, background_color);
  GRect bounds = layer_get_bounds(window_layer);

  layer = layer_create(bounds);
  layer_set_update_proc(layer, update_layer);
  layer_add_child(window_layer, layer);
  
  // Create time TextLayer
  //s_time_layer = text_layer_create(GRect(5, 55, 139, 50));
  //text_layer_set_background_color(s_time_layer, GColorClear);
  //text_layer_set_text_color(s_time_layer, GColorWhite);
  //text_layer_set_text(s_time_layer, "00:00");
  //text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  //s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_QUARK_ZONE_36));
  //text_layer_set_font(s_time_layer, s_time_font);
  //text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  //layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  //layer_set_hidden((Layer *) s_time_layer, true);
  
  // Create dateLayer
  s_date_layer = text_layer_create(GRect(50, 153, 50, 15));
  text_layer_set_background_color(s_date_layer, GColorBlack);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "Sat 01");
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
  
  // Create paramLayer
  //s_param_layer = text_layer_create(GRect(0, 154, 144, 14));
  //text_layer_set_background_color(s_param_layer, GColorBlack);
  //text_layer_set_text_color(s_param_layer, GColorWhite);
  //text_layer_set_text(s_param_layer, "Sat 01");
  //text_layer_set_font(s_param_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  //text_layer_set_text_alignment(s_param_layer, GTextAlignmentCenter);
  //layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_param_layer));
  
  // Hands layer
  s_hands_layer = layer_create(bounds);
  layer_set_update_proc(s_hands_layer, hands_update_proc);
  layer_add_child(window_layer, s_hands_layer);
  s_minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  s_hour_arrow = gpath_create(&HOUR_HAND_POINTS);
  GPoint center = grect_center_point(&bounds);
  gpath_move_to(s_minute_arrow, center);
  gpath_move_to(s_hour_arrow, center);

  update_time();
}


static void window_unload(Window *window) {
  layer_destroy(layer);
}

static void handle_init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = false;
  
  window_stack_push(window, animated);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  accel_tap_service_subscribe(tap_handler);
}


void handle_deinit(void) {
  //text_layer_destroy(s_time_layer);
  //text_layer_destroy(s_param_layer);
  text_layer_destroy(s_date_layer);
  gpath_destroy(s_minute_arrow);
  gpath_destroy(s_hour_arrow);

  window_destroy(window);
  tick_timer_service_unsubscribe();
  accel_tap_service_unsubscribe();
}


int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
