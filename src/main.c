#include "canvas.h"
#include "keys.h"
#include "space_obj.h"
#include "ticker.h"

int main(void)
{
	struct space_obj_type sot;
	sotype_init(&sot, SPACE_OBJ_PLAYER);
	*sotype_icon(&sot) = pixel('X', GREEN);
	invert_pixel(sotype_icon(&sot));
	*sotype_friction(&sot) = 0.99;
	*sotype_acceleration(&sot) = 0.03;
	*sotype_rotation(&sot) = 0.05;
	struct space_obj so;
	space_obj_init(&so, &sot);
	space_obj_pos(&so)->y = 50.0;
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
		lk = last_key(keybuf, 20);
		switch (lk) {
			case 'w':
				space_obj_thrust(&so);
				break;
			case 'a':
				space_obj_rleft(&so);
				break;
			case 'd':
				space_obj_rright(&so);
				break;
			case 'q':
				space_obj_rleft(&so);
				space_obj_thrust(&so);
				break;
			case 'e':
				space_obj_rright(&so);
				space_obj_thrust(&so);
				break;
			case '\04':
				goto CLEANUP;
		}
		space_obj_update(&so);
		space_obj_draw(&so, &c);
		canvas_print(&c, stderr);
		fflush(stderr);
		tick(&t);
		canvas_unprint(&c, stderr);
		space_obj_undraw(&so, &c);
	}

	CLEANUP:
	reset_single_key_input(&old_settings);
}

