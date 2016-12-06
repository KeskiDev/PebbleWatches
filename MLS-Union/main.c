#include <pebble.h>

#define NUM_PATHS 2

static const GPathInfo STRIPE ={
	4,
	(GPoint []) {
		{150,-5},
		{150,-25},
		{-5,170},
		{-5,190},
	}
};

static const GPathInfo TOP_CORNER ={
	4,
	(GPoint []) {
		{-5,-5},
		{150,-15},
		{-5,170},
		{-5,-15},
	}
};

static Window *s_the_main_window;

static TextLayer *clock_layer;
static TextLayer *date_layer;


static Layer *stripe_layer;
static Layer *top_corner_layer;
static Layer *circle_layer;

static BitmapLayer *s_background_2;
static GBitmap *s_background;



static GPath *bolt_path_array[NUM_PATHS];
static GPath *path1, *path2, *s_current_path, *s_second_path;

static void path_layer_update_callback(Layer *layer, GContext *ctx) {

	graphics_context_set_fill_color(ctx,GColorWhite);
	gpath_draw_filled(ctx,s_current_path);

	graphics_context_set_fill_color(ctx,COLOR_FALLBACK(GColorOxfordBlue, GColorBlack));
	gpath_draw_filled(ctx,s_second_path);
}


static void main_window_load(Window *window){
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);

		/******************************** SNAKE IMAGE ********************************************************/

	 s_background = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SNAKE);
  s_background_2 = bitmap_layer_create(GRect(65, 50, 150, 160));
  bitmap_layer_set_bitmap(s_background_2, s_background);
  layer_add_child(window_get_root_layer(window),bitmap_layer_get_layer(s_background_2));


	/********************************************************************************************************/

	/*************************** top right corner ***********************/
	top_corner_layer = layer_create(bounds);
	layer_set_update_proc(top_corner_layer,path_layer_update_callback);
	layer_add_child(window_layer, top_corner_layer);

	/******************************************************************************/


	/**************************** for the stripe ***************************/
	stripe_layer = layer_create(bounds);
	layer_set_update_proc(stripe_layer,path_layer_update_callback);
	layer_add_child(window_layer, stripe_layer);

	/************************************************************************/



	/****************************** clock *****************************************************/
	clock_layer = text_layer_create(GRect(0, 0, bounds.size.w, 50));
	text_layer_set_background_color(clock_layer,GColorClear);
	text_layer_set_text(clock_layer,"03:28");
	text_layer_set_text_color(clock_layer,COLOR_FALLBACK(GColorVividCerulean,GColorWhite));
	text_layer_set_font(clock_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
	text_layer_set_text_alignment(clock_layer, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(clock_layer));


	/********************************************************************************************************/

		/****************************** date *****************************************************/
	date_layer = text_layer_create(GRect(5, 45, 35, 30));
	text_layer_set_background_color(date_layer,GColorClear);
	text_layer_set_text(date_layer,"28");
	text_layer_set_text_color(date_layer,GColorWhite);
	text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
	text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
	/********************************************************************************************************/


	//#if defined(PBL_BW)
	//text_layer_set_text_alignment(clock_layer, GTextAlignmentLeft);
	//#elif defined(PBL_COLOR)
	//text_layer_set_text_alignment(clock_layer, GTextAlignmentLeft);
	//#elif defined(PBL_ROUND)
	//text_layer_set_text_alignment(clock_layer, GTextAlignmentCenter);
	//#endif


	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Width %d", bounds.size.w);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Height %d", bounds.size.h);

}

static void main_window_unload(Window *window){
	layer_destroy(stripe_layer);
	layer_destroy(top_corner_layer);
	layer_destroy(circle_layer);
	text_layer_destroy(clock_layer);
	text_layer_destroy(date_layer);
}

static void update_time(){
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	static char buffer[] = "00:00";

	if(clock_is_24h_style() == true){
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  	text_layer_set_text(clock_layer, buffer);

  }else{
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
		text_layer_set_text(clock_layer,buffer+(('0' == buffer[0])?1:0));

  }
}

static void update_date(){
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	static char buffer3[50];

	strftime(buffer3, 50, "%d", tick_time);

	text_layer_set_text(date_layer, buffer3);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
	update_time();
	update_date();
}


static void init(){

	s_the_main_window = window_create();

	window_set_background_color(s_the_main_window, COLOR_FALLBACK(GColorLimerick,GColorWhite));

	window_set_window_handlers(s_the_main_window, (WindowHandlers){
		.load = main_window_load,
		.unload = main_window_unload
	});

	window_stack_push(s_the_main_window,true);

	update_time();
	update_date();

	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

	path1 = gpath_create(&STRIPE);
	bolt_path_array[0] = path1;
	s_current_path = bolt_path_array[0];

	path2 = gpath_create(&TOP_CORNER);
	bolt_path_array[1] = path2;
	s_second_path = bolt_path_array[1];
}

static void deinit(){
	window_destroy(s_the_main_window);
}

int main(void){
	init();
	app_event_loop();
	deinit();
}
