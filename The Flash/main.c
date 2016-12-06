#include <pebble.h>

#define NUM_PATHS 2

static const GPathInfo LIGHTNING_BOLT ={
	6,
	(GPoint []) {
		{105,25},
		{40,100},
		{80,80},
		{30,150},
		{110,60},
		{75,75},
	}
};

static const GPathInfo REVERSE_BOLT ={
	6,
	(GPoint []) {
		{39,25},
		{104,100},
		{64,80},
		{114,150},
		{34,60},
		{69,75},
	}
};

static const GPathInfo ZOOM_BOLT ={
	6,
	(GPoint []) {
		{39,25},
		{104,100},
		{64,80},
		{114,150},
		{34,60},
		{69,75},
	}
};

static const GPathInfo ROUND_BOLT ={
	6,
	(GPoint []) {
		{125,25},
		{60,100},
		{100,80},
		{50,150},
		{130,60},
		{95,75},
	}
};

static Window *s_the_main_window;

static TextLayer *clock_layer;
static TextLayer *date_layer;
static TextLayer *day_layer;
static Layer *s_battery_layer;

static Layer *bolt_path_layer;
static Layer *bolt_outline_layer;
static Layer *circle_layer;

static GPath *bolt_path_array[NUM_PATHS];
static GPath *path1, /**path15,*/ *s_current_path, *s_second_path,*round_one,*current_round;

//when battery level is @ 20% it switches to reverse flash colors
/************************************** battery effect ********************************************/
	static int battery_level;
	static Layer *s_battery_layer;

static void battery_callback(BatteryChargeState state){
	battery_level = state.charge_percent;

	if(battery_level > 30){
		window_set_background_color(s_the_main_window, GColorRed);
	}
	else if(battery_level > 15){
		window_set_background_color(s_the_main_window, GColorYellow);
	}
	else{
		window_set_background_color(s_the_main_window, GColorBlack);
	}
}


	/*************************************************************************************************/

static void path_layer_update_callback(Layer *layer, GContext *ctx) {

	if(battery_level > 30){
		graphics_context_set_fill_color(ctx,GColorYellow);
		gpath_draw_filled(ctx,s_current_path);
	}
	else if(battery_level > 15){
		graphics_context_set_fill_color(ctx,GColorRed);
		gpath_draw_filled(ctx,s_current_path);
	}
	else{
		graphics_context_set_fill_color(ctx,GColorLightGray);
		gpath_draw_filled(ctx,s_current_path);
	}
}


static void canvas_update_proc(Layer *this_layer, GContext *ctx){
	GRect bounds = layer_get_bounds(this_layer);

	if(battery_level > 30){
		graphics_context_set_fill_color(ctx,GColorBlack);
		graphics_fill_circle(ctx,GPoint(bounds.size.w/2, bounds.size.h/2),32);

		graphics_context_set_fill_color(ctx,GColorWhite);
		graphics_fill_circle(ctx,GPoint(bounds.size.w/2, bounds.size.h/2),30);

		text_layer_set_text_color(clock_layer,GColorBlack);
		text_layer_set_text_color(date_layer,GColorYellow);
		text_layer_set_text_color(day_layer,GColorYellow);
	}
	else if(battery_level  > 15){
		graphics_context_set_fill_color(ctx,GColorRed);
		graphics_fill_circle(ctx,GPoint(bounds.size.w/2, bounds.size.h/2),32);

		graphics_context_set_fill_color(ctx,GColorBlack);
		graphics_fill_circle(ctx,GPoint(bounds.size.w/2, bounds.size.h/2),32);

		//clock_layer = text_layer_create(GRect(0, bounds.size.h-80, bounds.size.w, 80));
		text_layer_set_text_color(clock_layer,GColorRed);
		text_layer_set_text_color(date_layer,GColorRed);
		text_layer_set_text_color(day_layer,GColorRed);
	}
	else{
		graphics_context_set_fill_color(ctx,GColorLightGray);
		graphics_fill_circle(ctx,GPoint(bounds.size.w/2, bounds.size.h/2),32);

		graphics_context_set_fill_color(ctx,GColorBlack);
		graphics_fill_circle(ctx,GPoint(bounds.size.w/2, bounds.size.h/2),30);

		text_layer_set_text_color(clock_layer,GColorBlue);
		text_layer_set_text_color(date_layer,GColorBlue);
		text_layer_set_text_color(day_layer,GColorBlue);
	}

}

static void main_window_load(Window *window){
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);

	/*************************** circle behind the bolt ***********************/
	circle_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
	layer_add_child(window_layer,circle_layer);
	layer_set_update_proc(circle_layer, canvas_update_proc);

	/******************************************************************************/


	/**************************** for the lightning bolt ***************************/
	bolt_path_layer = layer_create(bounds);
	layer_set_update_proc(bolt_path_layer,path_layer_update_callback);
	layer_add_child(window_layer, bolt_path_layer);

	/************************************************************************/



	/****************************** clock *****************************************************/
	clock_layer = text_layer_create(GRect(0, 0, bounds.size.w, 80));
	text_layer_set_background_color(clock_layer,GColorClear);
	text_layer_set_text(clock_layer,"03:28");
	text_layer_set_text_color(clock_layer,GColorBlack);
	text_layer_set_font(clock_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
	text_layer_set_text_alignment(clock_layer, GTextAlignmentCenter);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(clock_layer));
	/********************************************************************************************************/

	/******************************* day of the week *****************************************************/

	date_layer = text_layer_create(GRect(0,bounds.size.h-40,bounds.size.h/2-15,40));
	text_layer_set_background_color(date_layer,GColorClear);
	text_layer_set_text(date_layer,"Wed");
	text_layer_set_text_color(date_layer,GColorYellow);
	text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_alignment(date_layer, GTextAlignmentRight);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));


	/********************************************************************************************/


	/********************************** day of the month ********************************************************/
	day_layer = text_layer_create(GRect(bounds.size.w/2+5,bounds.size.h-40,bounds.size.h/2-10,40));
	text_layer_set_background_color(day_layer,GColorClear);
	text_layer_set_text(day_layer,"26");
	text_layer_set_text_color(day_layer,GColorYellow);
	text_layer_set_font(day_layer, fonts_get_system_font(FONT_KEY_LECO_28_LIGHT_NUMBERS));
	text_layer_set_text_alignment(day_layer, GTextAlignmentLeft);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(day_layer));



	/***********************************************************************************************************/





	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Width %d", bounds.size.w);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Height %d", bounds.size.h);

}

static void main_window_unload(Window *window){
	layer_destroy(bolt_path_layer);
	layer_destroy(bolt_outline_layer);
	layer_destroy(circle_layer);
	text_layer_destroy(clock_layer);
}

static void update_time(){
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	static char buffer[] = "00:00";

	if(clock_is_24h_style() == true){
		strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
	}
	else{
		strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
	}

	text_layer_set_text(clock_layer, buffer);
}

static void update_day_of_the_month(){
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	static char buffer2[10];

	strftime(buffer2, 10, "%d", tick_time);

	text_layer_set_text(day_layer, buffer2);
}

static void update_day_of_the_week(){
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	static char buffer3[15];

	strftime(buffer3, 15, "%a", tick_time);

	text_layer_set_text(date_layer, buffer3);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
	update_time();
	update_day_of_the_month();
	update_day_of_the_week();
}


static void init(){



	s_the_main_window = window_create();

	window_set_background_color(s_the_main_window, GColorRed);

	window_set_window_handlers(s_the_main_window, (WindowHandlers){
		.load = main_window_load,
		.unload = main_window_unload
	});

	window_stack_push(s_the_main_window,true);
	update_time();
	update_day_of_the_month();
	update_day_of_the_week();

	battery_state_service_subscribe(battery_callback);
	battery_callback(battery_state_service_peek());

	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);



		if(battery_level > 20){
			path1 = gpath_create(&LIGHTNING_BOLT);
			bolt_path_array[0] = path1;
			s_current_path = bolt_path_array[0];
		}
		else if(battery_level > 10){
			path1 = gpath_create(&REVERSE_BOLT);
			bolt_path_array[0] = path1;
			s_current_path = bolt_path_array[0];
		}
		else{
			path1 = gpath_create(&ZOOM_BOLT);
			bolt_path_array[0] = path1;
			s_current_path = bolt_path_array[0];
		}


}

static void deinit(){
	window_destroy(s_the_main_window);
}

int main(void){
	init();
	app_event_loop();
	deinit();
}
