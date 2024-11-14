#include "allegro5/allegro.h"
#include "allegro5/allegro_font.h"
#include "allegro5/allegro_ttf.h"
#include "allegro5/allegro_primitives.h"

/*
first, any touch
text needs to appear
text needs to move
text needs to be destroyed
*/

//
// data
//

#define GRAVITY 100
#define IN_HEART 1050
#define SCREEN_HALF 540
#define MAX 80

typedef struct Vector
{
	double x = 0.f, y = 0.f, z = 0.f;
} vector;

struct Shape
{
	int n = 9;
	ALLEGRO_VERTEX *v;
} heart;

struct MyText
{
	// added
	ALLEGRO_COLOR c1 = al_map_rgba(
		100.f, 50.f, 100.f, 255.f);
	int x = 540.f, y = 1050;
	float dx = 0.f, dy = 0.f;
	double duration = 6.f;
	double started = 0.f;
	int alive = 0;
} test_text;


//
// functions
//

void setPerspective()
{
	ALLEGRO_TRANSFORM projection;
	ALLEGRO_DISPLAY *display =
		al_get_current_display();

	double sw = al_get_display_width(display);
	double sh = al_get_display_height(display);
	// radians
	float f = 90 * ALLEGRO_PI / 180;

	// must identity
	al_identity_transform(&projection);
	
	// example from projection2.c
	// 0,0,0 in centre of screen
	al_perspective_transform(&projection,
	(-1 * sw / sh * f), f, 1,
	(f * sw / sh), -f, 1000);

	al_use_projection_transform(&projection);
}

void set2d()
{
	ALLEGRO_TRANSFORM p;
	al_identity_transform(&p);
	al_orthographic_transform(
		&p, -1,
		0,0,
		1080, 2000, 1);
	al_use_projection_transform(&p);
}


//
// make
//

void makeShape()
{
	heart.v = (ALLEGRO_VERTEX *)realloc(heart.v, heart.n * sizeof * heart.v);
	
	float depth = 0.f;
	float x1 = -5.f;
	float x2 = -3.f;
	float x3 = 3.f;
	float x4 = 5.f;
	float y1 = -6.f;
	float y2 = 2.f;

	ALLEGRO_COLOR c1 = al_map_rgb(
		255.f, 80.f, 80.f);

	// top left
	heart.v[0] = {.x = x1, .y = 0.f, .z = depth, .u = 20.f, .v = 20., .color = c1};

	heart.v[1] = {.x = x2, .y = y2, .z = depth, .u = 20.f, .v = 20.f, .color = c1};

	heart.v[2] = {.x = 0.f, .y = 0.f, .z = depth, .u = 20.f, .v = 20.f, .color = c1};
	
	// top right
		heart.v[3]= {.x = x4, .y = 0.f, .z = depth, .u = 20.f, .v = 20., .color = c1};

	heart.v[4] = {.x = x3, .y = y2, .z = depth, .u = 20.f, .v = 20.f, .color = c1};

	heart.v[5] = {.x = 0.f, .y = 0.f, .z = depth, .u = 20.f, .v = 20.f, .color = c1};
	
	// bottom
	heart.v[6] = heart.v[0];
	heart.v[7] = heart.v[3];
	heart.v[8] = {.x = 0.f, .y = y1, .z = depth, .u = 20.f, .v = 20.f, .color = c1 };
}

//
// move
//

float returnSign(
float amount)
{
	return sin(al_get_time() * 6) * amount;
}

float getDirection()
{
	return ((rand() % 300) - 150) / 50.f;
}

float getGravity(
ALLEGRO_TIMER *timer)
{
	return GRAVITY * al_get_timer_speed(timer);
}

void loseColour(
int id,
MyText *t)
{
	double now = al_get_time();
	double delta = now - t[id].started;
	double amount = 255.f - (delta/t[id].duration*255.f);
	if(amount<0)
	{
		amount = 0.f;
		t[id].alive = 0;
	}
	t[id].c1 = al_premul_rgba(100.f, 50.f, 100.f, amount);
}

void makeText(
ALLEGRO_FONT *font,
int x, int y)
{
	al_draw_text(
		font, test_text.c1, x, y,
		ALLEGRO_ALIGN_CENTER,
		"MOLLY");
}

void makeLots(
MyText *texts,
ALLEGRO_FONT *font,
ALLEGRO_TIMER *timer)
{
	for(int i=0; i<MAX; ++i)
	{
		// pre check!
		if(texts[i].alive)
		{
		loseColour(i, texts);
		
		texts[i].x += texts[i].dx;
		texts[i].x += returnSign(5.f);
		texts[i].y -= getGravity(timer);
		
		al_draw_text(font, texts[i].c1,
			texts[i].x,
			texts[i].y,
			ALLEGRO_ALIGN_CENTER,
			"MOLLY");
		}
	}
}

void addHeart(int &id, MyText *t)
{
	t[id].alive = 1;
	t[id].started = al_get_time();
	t[id].dx = getDirection();
	t[id].x = SCREEN_HALF;
	t[id].y = IN_HEART;
	
	if(id<MAX)
		id++;
	else
		id = 0;
}

//
// draw
//

void drawScene(
ALLEGRO_TIMER *timer,
ALLEGRO_FONT *font,
MyText *hearts)
{
	setPerspective();

	al_clear_to_color(
		al_map_rgb(255.f, 200.f, 200.f));
	al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
	al_clear_depth_buffer(1);

	ALLEGRO_TRANSFORM camera;
	
	// no identity needed

	al_build_camera_transform(
		&camera,
		0, 0, 10,
		0, 0, 0,
		0, 1, 0);
	al_use_transform(&camera);

	al_draw_prim(heart.v, NULL, NULL,
				 0, heart.n + 1,
				 ALLEGRO_PRIM_TRIANGLE_LIST);
				 
	// need 2d orth
	al_identity_transform(&camera);
	al_use_transform(&camera);
	al_set_render_state(ALLEGRO_DEPTH_TEST,0);
	set2d();
	
	makeLots(hearts, font, timer);
	
	// testing
	//float x = returnSign(timer, 50.f);
	//makeText(font, 540 + x, 1050);
}

//
// main
//

int main(int argc, char *argv[])
{
	al_init();
	al_init_primitives_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	
	al_install_touch_input();

	ALLEGRO_DISPLAY *display =
		al_create_display(720, 1600);
	ALLEGRO_TIMER *timer =
		al_create_timer(1.0f / 30.0f);
	ALLEGRO_EVENT_QUEUE *queue =
		al_create_event_queue();

	ALLEGRO_FONT *font = al_load_ttf_font("/system/fonts/NotoSerif-Bold.ttf", 60, 0);
	if(!font)
		return 1;
		
	ALLEGRO_EVENT event;
	
	al_register_event_source(queue,
		al_get_touch_input_event_source());
		
	al_register_event_source(
		queue,
		al_get_timer_event_source(timer));
		
	bool running = true;
	bool redraw = false;
	
	// heart
	MyText hearts[500];
	static int id = 0;
	srand(time(0));

	makeShape();

	al_start_timer(timer);

	while (running)
	{
		al_wait_for_event(queue, &event);

		switch (event.type)
		{
		case ALLEGRO_EVENT_TIMER:
		{
			redraw = true;
			break;
		}
		case ALLEGRO_EVENT_DISPLAY_CLOSE:
		{
			running = false;
			break;
		}
		case ALLEGRO_EVENT_DISPLAY_HALT_DRAWING:
		{
			al_stop_timer(timer);
			redraw = false;
			al_acknowledge_drawing_halt(display);
			break;
		}
		case ALLEGRO_EVENT_DISPLAY_RESUME_DRAWING:
		{
			al_acknowledge_drawing_resume(display);
			al_start_timer(timer);
			break;
		}
		case ALLEGRO_EVENT_TOUCH_BEGIN:
		{
			addHeart(id, hearts);
			break;
		}
		}
		
		//
		// draw
		//

		if (redraw &&
			al_event_queue_is_empty(queue))
		{
			drawScene(timer, font, hearts);
			al_flip_display();
			redraw = false;
		}
	}

	al_destroy_event_queue(queue);
	al_destroy_font(font);
	al_destroy_display(display);

	return 0;
}