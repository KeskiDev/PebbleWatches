#include <pebble.h>
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_day_layer;
static GFont s_time_font;
static GFont s_date_font;
static GFont s_day_font;
static BitmapLayer *s_background_2;
static GBitmap *s_background;

static void main_window_load(Window *window){
  //creating the clone trooper helmet image layer
  s_background = gbitmap_create_with_resource(RESOURCE_ID_FIRST_ORDER);
  s_background_2 = bitmap_layer_create(GRect(70, 0, 150, 160));
  bitmap_layer_set_bitmap(s_background_2, s_background);
  layer_add_child(window_get_root_layer(window),bitmap_layer_get_layer(s_background_2));


  //create time text layer
  s_time_layer = text_layer_create(GRect(5,120,120,80));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text_color(s_time_layer, GColorWhite);

  //create date text layer
  s_date_layer = text_layer_create(GRect(10, 5, 60, 30));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorWhite);
  text_layer_set_text(s_date_layer, "00/00");

	//create the layer for the day
	s_day_layer = text_layer_create(GRect(10, 40, 60, 30));
	text_layer_set_background_color(s_day_layer, GColorClear);
	text_layer_set_text_color(s_day_layer, GColorWhite);
  text_layer_set_text(s_day_layer, "Sat");

	//s_time was size 30
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_STARWARS_38));
	s_day_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_STARWARS_20));


  //apply textlayer clock
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentLeft);

  //apply to date textlayer
   text_layer_set_font(s_date_layer, s_day_font);
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);

	//apply to day textlayer
	text_layer_set_font(s_day_layer, s_day_font);
  text_layer_set_text_alignment(s_day_layer, GTextAlignmentLeft);

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

  }else{
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);

  }

  text_layer_set_text(s_time_layer, buffer);
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
  //creates main window element and assign to pointer
  s_main_window = window_create();
	window_set_background_color(s_main_window, GColorBlack);


  //set handlers to manage the elements inside the window
  window_set_window_handlers(s_main_window, (WindowHandlers){
    .load = main_window_load,
    .unload = main_window_unload
  });

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
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
