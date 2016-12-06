#include "simple_analog.h"

#include "pebble.h"

#include "string.h"
#include "stdlib.h"

Layer *simple_bg_layer;
Layer *mealTime_layer;
Layer *date_layer;
TextLayer *day_label;
char day_buffer[6];
TextLayer *num_label;
char num_buffer[4];
char what_to_eat[50];

static GPath *minute_arrow;
static GPath *hour_arrow;
static GPath *tick_paths[NUM_CLOCK_TICKS];
Layer *hands_layer;
Window *window;
static TextLayer *foodTime;

//batterycallback
//each dot update proc



static void bg_update_proc(Layer *layer, GContext *ctx) {

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);

  graphics_context_set_fill_color(ctx, GColorWhite);
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_draw_filled(ctx, tick_paths[i]);
  }
}

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  // minute/hour hand
  graphics_context_set_fill_color(ctx, COLOR_FALLBACK(GColorRed, GColorWhite));
  graphics_context_set_stroke_color(ctx, COLOR_FALLBACK(GColorRed, GColorWhite));

  gpath_rotate_to(minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, minute_arrow);
  gpath_draw_outline(ctx, minute_arrow);

  gpath_rotate_to(hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, hour_arrow);
  gpath_draw_outline(ctx, hour_arrow);

  // dot in the middle
	graphics_context_set_fill_color(ctx, COLOR_FALLBACK(GColorYellow, GColorWhite));
 	graphics_fill_circle(ctx, GPoint(bounds.size.w/2, bounds.size.h/2),9);
	//so the circle has a black center
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, GPoint(bounds.size.w/2, bounds.size.h/2),7);



	}
}

static void txtLayer_update(){


	APP_LOG(APP_LOG_LEVEL_DEBUG, "trying something else");
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "%c%c%c%c%c %c%c",  buffer[0], buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6]);
	//get a time structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  //create a long lived buffer
  static char buffer[15];

  //write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true){
    strftime(buffer, 15, "%R", tick_time);

		//future add a function that will handle the changing the layer text

		switch(buffer[0]){
			case '0':
				if(buffer[1]=='0'){
					text_layer_set_text(foodTime, "midnight snack");
				}
				else if(buffer[1]=='7'){
					text_layer_set_text(foodTime, "breakfast");
				}
				else if(buffer[1]=='9'){
					text_layer_set_text(foodTime, "2nd breakfast");
				}
				break;

			case '1':
				if(buffer[1]=='1'){
					text_layer_set_text(foodTime, "elevenses");
				}
				else if(buffer[1]=='3'){
					text_layer_set_text(foodTime, "lunch");
				}
				else if(buffer[1]=='5'){
					text_layer_set_text(foodTime, "afternoon tea");
				}
				else if(buffer[1]=='8'){
					text_layer_set_text(foodTime, "dinner");
				}
				break;

			case '2':
				if(buffer[1]=='0'){
					text_layer_set_text(foodTime, "supper");
				}
				break;

			default:
				text_layer_set_text(foodTime, "What about 2nd breakfast?");
				break;
		}
	}
	else{
    strftime(buffer, 15, "%r", tick_time);
		/*for(int j=0; j<=15; j++){
			APP_LOG(APP_LOG_LEVEL_DEBUG, "%i: %c",  j, buffer[j]);
		}*/
		//future there will be a function

		switch(buffer[0]){
			case '0':
				if(buffer[1]=='7' && buffer[9] == 'A'){
					text_layer_set_text(foodTime, "breakfast");
				}
				else if(buffer[1]=='9' && buffer[9] == 'A'){
					text_layer_set_text(foodTime, "2nd breakfast");
				}
				else if(buffer[1]=='1' && buffer[9] == 'P'){
					text_layer_set_text(foodTime, "lunch");
				}
				else if(buffer[1]=='3' && buffer[9] == 'P'){
					text_layer_set_text(foodTime, "afternoon tea");
				}
				else if(buffer[1]=='6' && buffer[9] == 'P'){
					text_layer_set_text(foodTime, "dinner");
				}
				else if(buffer[1]=='8' && buffer[9] == 'P'){
					text_layer_set_text(foodTime, "supper");
				}
				break;

			case '1':
				if(buffer[1]=='1' && buffer[9] == 'A'){
					text_layer_set_text(foodTime, "elevenses");
				}
				else if(buffer[1]=='3' && buffer[9] == 'P'){
					text_layer_set_text(foodTime, "lunch");
				}
				else if(buffer[1]=='5' && buffer[9] == 'P'){
					text_layer_set_text(foodTime, "afternoon tea");
				}
				else if(buffer[1]=='8' && buffer[9] == 'P'){
					text_layer_set_text(foodTime, "dinner");
				}
				else if(buffer[1] == '2' && buffer[9] == 'A'){
					text_layer_set_text(foodTime, "midnight snack");
				}
				break;
			default:
				text_layer_set_text(foodTime, "What about 2nd breakfast?");
				break;
		}

}


static void date_update_proc(Layer *layer, GContext *ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  strftime(num_buffer, sizeof(num_buffer), "%d", t);
  text_layer_set_text(num_label, num_buffer);
}
//in the load, place and remove layer
static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // init layers
  simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, simple_bg_layer);

  // init date layer -> a plain parent layer to create a date update proc
  date_layer = layer_create(bounds);
  layer_set_update_proc(date_layer, date_update_proc);
  layer_add_child(window_layer, date_layer);

  // the LOTR "logo"
  day_label = text_layer_create(GRect(5, bounds.size.h/2-12, 27, 24));
  text_layer_set_text(day_label, "lotr");
  text_layer_set_background_color(day_label, GColorClear);
  text_layer_set_text_color(day_label, COLOR_FALLBACK(GColorYellow,GColorWhite));

  GFont lotr = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LOTR_15));
 	GFont hobbit = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_HOBBITS_17));

	text_layer_set_font(day_label, hobbit);

  layer_add_child(date_layer, text_layer_get_layer(day_label));

  // init num
  num_label = text_layer_create(GRect(65, 140, 18, 20));

  text_layer_set_text(num_label, num_buffer);
  text_layer_set_background_color(num_label, GColorClear);
  text_layer_set_text_color(num_label, GColorWhite);
  GFont bold18 = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(num_label, bold18);
  layer_add_child(date_layer, text_layer_get_layer(num_label));

  // init hands
  hands_layer = layer_create(bounds);
  layer_set_update_proc(hands_layer, hands_update_proc);
  layer_add_child(window_layer, hands_layer);

	mealTime_layer = layer_create(bounds);

	//the layer for the meals
	foodTime = text_layer_create(GRect(12, 100, 120, 55));
	text_layer_set_background_color(foodTime, GColorClear);
	text_layer_set_text_color(foodTime, GColorWhite);
	//text_layer_set_text(foodTime, "what about 2nd breakfast?");
	text_layer_set_text_alignment(foodTime, GTextAlignmentCenter);
	text_layer_set_font(foodTime, lotr);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(foodTime));




}

static void window_unload(Window *window) {
  layer_destroy(simple_bg_layer);
  layer_destroy(date_layer);
  text_layer_destroy(day_label);
  text_layer_destroy(num_label);
  layer_destroy(hands_layer);

	text_layer_destroy(foodTime);
}



static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
	txtLayer_update();
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });

  num_buffer[0] = '\0';

  // init hand paths
  minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  hour_arrow = gpath_create(&HOUR_HAND_POINTS);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  const GPoint center = grect_center_point(&bounds);
  gpath_move_to(minute_arrow, center);
  gpath_move_to(hour_arrow, center);

  // init clock face paths
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    tick_paths[i] = gpath_create(&ANALOG_BG_POINTS[i]);
  }

  // Push the window onto the stack
  const bool animated = true;
  window_stack_push(window, animated);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_second_tick);

	txtLayer_update();
}

static void deinit(void) {
	gpath_destroy(minute_arrow);
  gpath_destroy(hour_arrow);

  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_destroy(tick_paths[i]);
  }

  tick_timer_service_unsubscribe();
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
