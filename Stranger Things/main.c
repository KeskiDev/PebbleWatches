#include <pebble.h>
#define NUM_PATHS 3

// This defines graphics path information to be loaded as a path later

static const GPathInfo PATH_POINTS = {
 	4,
  (GPoint []) {
    {4,30},
		{140,30},
		{140,35},
		{4,35},
	}
};

static const GPathInfo WHITE_STRIPE = {
 	4,
  (GPoint []) {
    {102,85},
		{140,85},
		{140,90},
		{102,90},
	}
};

static const GPathInfo THIRD_ONE = {
 	4,
  (GPoint []) {
    {4,85},
		{42,85},
		{42,90},
		{4,90},
	}
};

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_day_layer;
static GFont s_time_font;
static GFont s_date_font;
static GFont s_day_font;
static GBitmap *s_background;

static Layer *s_path_layer;

static GPath *s_path_array[NUM_PATHS];
static GPath *s_red_path,*s_current_path,*s_white_path,*s_second_path, *s_thrid,*s_third_path;

// This is the layer update callback which is called on render updates
static void path_layer_update_callback(Layer *layer, GContext *ctx) {
	graphics_context_set_stroke_color(ctx, COLOR_FALLBACK(GColorRed, GColorWhite));
  gpath_draw_outline(ctx, s_current_path);

	graphics_context_set_stroke_color(ctx, COLOR_FALLBACK(GColorRed, GColorWhite));
  gpath_draw_outline(ctx, s_second_path);

	graphics_context_set_stroke_color(ctx, COLOR_FALLBACK(GColorRed, GColorWhite));
  gpath_draw_outline(ctx, s_third_path);
}

static void accel_data_handler(AccelData *data, uint32_t num_samples) {
  // Read sample 0's x, y, and z values
  int16_t x = data[0].x;
  int16_t y = data[0].y;
  int16_t z = data[0].z;

  // Determine if the sample occured during vibration, and when it occured
  bool did_vibrate = data[0].did_vibrate;
  uint64_t timestamp = data[0].timestamp;

  if(!did_vibrate) {
    // Print it out
   APP_LOG(APP_LOG_LEVEL_INFO, "t: %llu, x: %d, y: %d, z: %d", timestamp, x, y, z);
		if(y > 900 || y < -900){
			text_layer_set_text_color(s_time_layer, COLOR_FALLBACK(GColorWhite, GColorWhite));
		}
		else if(y <= 900 && y >=-900){
			text_layer_set_text_color(s_time_layer, COLOR_FALLBACK(GColorRed, GColorWhite));
		}
  } else {
    // Discard with a warning
    APP_LOG(APP_LOG_LEVEL_WARNING, "Vibration occured during collection");
  }
}

static void main_window_load(Window *window){

	Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  //create time text layer
  s_time_layer = text_layer_create(GRect(0 ,bounds.size.h/2-60,144,60));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text_color(s_time_layer, COLOR_FALLBACK(GColorRed, GColorWhite));

  //create date text layer
  s_date_layer = text_layer_create(GRect(0, 140, 144, 30));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer,GColorWhite);
  text_layer_set_text(s_date_layer, "00/00");

	//create the layer for the day
	s_day_layer = text_layer_create(GRect(0, bounds.size.h/2-15, 144, 30));
	text_layer_set_background_color(s_day_layer, GColorClear);
	text_layer_set_text_color(s_day_layer, COLOR_FALLBACK(GColorRed, GColorWhite));
  text_layer_set_text(s_day_layer, "Sun");

  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_50));
	s_day_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_30));
	s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_20));


	//for the red bars
	s_path_layer = layer_create(bounds);
  layer_set_update_proc(s_path_layer, path_layer_update_callback);
  layer_add_child(window_layer, s_path_layer);


  //apply textlayer clock
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  //apply to date textlayer
  text_layer_set_font(s_date_layer, s_date_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

	//apply to day textlayer
	text_layer_set_font(s_day_layer, s_day_font);
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);

  //add it as a child layer to the window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_layer));
}

static void main_window_unload(Window *window){
  //destory textlayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
	text_layer_destroy(s_day_layer);

  //text_layer_set_font(s_time_layer,s_time_font);

  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_date_font);
	fonts_unload_custom_font(s_day_font);


	//(s_background_bitmap);
  gbitmap_destroy(s_background);

}



static void update_time(){
  //get a tim structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  //create a long lived buffer
  static char buffer[] = "00:00";


  //write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true){
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  	text_layer_set_text(s_time_layer, buffer);

  }else{
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
		text_layer_set_text(s_time_layer,buffer+(('0' == buffer[0])?1:0));

  }

}

static void update_date(){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char buffer2[] = "00/00";

  strftime(buffer2, sizeof("00/00"), "%m/%d",tick_time);

  text_layer_set_text(s_date_layer, buffer2);
}

static void update_day(){
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char buffer3[80];

  strftime(buffer3, 80, "%a", tick_time);

  text_layer_set_text(s_day_layer, buffer3);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
 	update_time();
  update_date();
	update_day();
}

static void init(){
	s_red_path = gpath_create(&PATH_POINTS);
	s_white_path = gpath_create(&WHITE_STRIPE);
	s_thrid = gpath_create(&THIRD_ONE);
	s_path_array[0] = s_red_path;
	s_path_array[1] = s_white_path;
	s_path_array[2] = s_thrid;
	s_current_path = s_path_array[0];
	s_second_path = s_path_array[1];
	s_third_path = s_path_array[2];

  //creates main window element and assign to pointer
  s_main_window = window_create();
	window_set_background_color(s_main_window, GColorBlack);


  //set handlers to manage the elements inside the window
  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	uint32_t num_samples = 5;  // Number of samples per batch/callback

	// Subscribe to batched data events
	accel_data_service_subscribe(num_samples, accel_data_handler);

  //tick_timer_service_subscribe(M, TickHandler handler)

  //show the window on the watch, with animated = true
  window_stack_push(s_main_window, true);

  update_time();
  update_date();
	update_day();
}
static void deinit(){
  //destroy window
  window_destroy(s_main_window);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}
