#include "canvas.h"
#include "keys.h"
#include "space_obj.h"
#include "ticker.h"

int main(void)
{
	struct space_obj_type sot;
	sot.icon = pixel('X', GREEN);
	invert_pixel(&sot.icon);
	sot.friction = 0.96;
	sot.acceleration = 0.02;
	sot.rotation = 0.05;
	struct space_obj so;
	so.type = &sot;
	so.direction = 0.0;
	so.pos.x = 0.0;
	so.vel.x = 0.0;
	so.pos.y = 50.0;
	so.vel.y = 0.0;
	struct canvas c;
	canvas_init(&c, 200, 50, EMPTY_SPACE_ICON);

	struct ticker t;
	ticker_init(&t, CLOCK_REALTIME, 0, 1000000000 / 30);

	char buf[BUFSIZ];
	setbuf(stderr, buf);

	struct termios old_settings;

	set_single_key_input(&old_settings);

	char keybuf[20];
	char lk;
	while (1) {
//		space_obj_undraw(&so, &c);
		lk = last_key(keybuf, 20);
		switch (lk) {
/*			case 'A':
				space_obj_thrust(&so);
				break;
*/			case 'D':
				space_obj_rleft(&so);
				break;
			case 'C':
				space_obj_rright(&so);
				break;
			case '\04':
				goto LOOP_END;
		}
		space_obj_thrust(&so);
		space_obj_update(&so);
		space_obj_draw(&so, &c);
		canvas_print(&c, stderr);
		fflush(stderr);
		tick(&t);
	}
	LOOP_END:

	reset_single_key_input(&old_settings);
}

