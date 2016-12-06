#include <pebble.h>

#define NUM_PATHS 2

// This defines graphics path information to be loaded as a path later

static const GPathInfo PATH_POINTS = {
 	4,
  (GPoint []) {
    {-20,10},
		{150,100},
		{150,110},
		{-20,20},
	}
};

static const GPathInfo WHITE_STRIPE = {
 	4,
  (GPoint []) {
    {-20,5},
		{150,90},
		{160,-5},
		{-5,-5},
	}
};

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_minute_layer;
static BitmapLayer *s_background_2;
static GBitmap *s_background;

static int s_battery_level;
static Layer *s_battery_layer;

static Layer *s_path_layer;

static GPath *s_path_array[NUM_PATHS];
static GPath *s_red_path,*s_current_path,*s_white_path,*s_second_path;

// This is the layer update callback which is called on render updates
static void path_layer_update_callback(Layer *layer, GContext *ctx) {

	graphics_context_set_fill_color(ctx, GColorWhite);
	gpath_draw_filled(ctx,s_current_path);

	graphics_context_set_fill_color(ctx, COLOR_FALLBACK(GColorRed, GColorWhite));
	gpath_draw_filled(ctx,s_second_path);
}


static void battery_callback(BatteryChargeState state){
	//record the new level
	s_battery_level = state.charge_percent;

	//update meter
	layer_mark_dirty(s_battery_layer);
}

static void battery_update_proc(Layer *layer, GContext *ctx){
	GRect bounds = layer_get_bounds(layer);

	//find width of bar ----- background color of the bar, what gets shows as battery dies.
	int width = (int)(float)(((float)s_battery_level/100.0F)*114.0F);
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, bounds, 0, GCornerNone);

	// Draw the bar --- current percentage of battery left
  graphics_context_set_fill_color(ctx, GColorGreen);
  graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);

}

static void main_window_load(Window *window){

	Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

	/****************************************************************************************************/
	//creating the soccer logo image layer
  s_background = gbitmap_create_with_resource(RESOURCE_ID_logo);
  s_background_2 = bitmap_layer_create(GRect(bounds.origin.x/2-52, -50, 250, 250));
	bitmap_layer_set_compositing_mode(s_background_2, GCompOpSet);
  bitmap_layer_set_bitmap(s_background_2, s_background);

	/*****************************************************************************************************/

	/****************** for the red and white colors *******************************************/

	s_path_layer = layer_create(bounds);
  layer_set_update_proc(s_path_layer, path_layer_update_callback);
  layer_add_child(window_layer, s_path_layer);

	/*********************************************************************************************/


  //create time text layer
  s_time_layer = text_layer_create(GRect(5,bounds.size.h-55,60,55));
  text_layer_set_background_color(s_time_layer, GColorClear);
 	text_layer_set_text(s_time_layer, "03");
  text_layer_set_text_color(s_time_layer, GColorWhite);

  //create date text layer
	s_minute_layer = text_layer_create(GRect(80, bounds.size.h-55, 60, 55));
  text_layer_set_background_color(s_minute_layer, GColorClear);
  text_layer_set_text_color(s_minute_layer, GColorWhite);
  text_layer_set_text(s_minute_layer, "30");

  //apply textlayer clock
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);

	text_layer_set_font(s_minute_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
  text_layer_set_text_alignment(s_minute_layer, GTextAlignmentRight);

  //add it as a child layer to the window's root layer
	layer_add_child(window_get_root_layer(window),bitmap_layer_get_layer(s_background_2));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_minute_layer));
	//layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_layer));

	//for the battery meter
	s_battery_layer = layer_create(GRect(5,25,2,115));
	layer_set_update_proc(s_battery_layer, battery_update_proc);
	//layer_add_child(window_layer, s_battery_layer);
}

static void main_window_unload(Window *window){
	layer_destroy(s_path_layer);

  //destory textlayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_minute_layer);

	//layer_destroy(s_battery_layer);
}

static void update_time(){
  //get a tim structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  //create a long lived buffer
  static char buffer[] = "00";


  //write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true){
    strftime(buffer, sizeof("00"), "%H", tick_time);

  }else{
    strftime(buffer, sizeof("00"), "%I", tick_time);

  }

  text_layer_set_text(s_time_layer, buffer);
}

static void update_minute(){
 	time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  static char buffer2[] = "00";

  strftime(buffer2, sizeof("00"), "%M",tick_time);

  text_layer_set_text(s_minute_layer, buffer2);
}


static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
 	update_time();
 	update_minute();
}

static void init(){
	s_red_path = gpath_create(&PATH_POINTS);
	s_white_path = gpath_create(&WHITE_STRIPE);
	s_path_array[0] = s_red_path;
	s_path_array[1] = s_white_path;
	s_current_path = s_path_array[0];
	s_second_path = s_path_array[1];


  //creates main window element and assign to pointer
  s_main_window = window_create();
	window_set_background_color(s_main_window, COLOR_FALLBACK(GColorDukeBlue, GColorBlack));


  //set handlers to manage the elements inside the window
  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });



  //show the window on the watch, with animated = true
  window_stack_push(s_main_window, true);

 	update_time();
  update_minute();

	battery_state_service_subscribe(battery_callback);
	battery_callback(battery_state_service_peek());
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
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
