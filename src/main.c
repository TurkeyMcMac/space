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
	*sotype_reload(&sot) = 99;
	*sotype_friction(&sot) = 0.99;
	*sotype_acceleration(&sot) = 0.03;
	*sotype_rotation(&sot) = 0.05;
	struct space_obj_node *sos = calloc(1, sizeof(struct space_obj_node));
	struct space_obj *so = &sos->so;
	space_obj_init(so, &sot);
	space_obj_pos(so)->y = 50.0;
	struct canvas c;
	canvas_init(&c, 200, 50, EMPTY_SPACE_ICON);

	struct ticker t;
	ticker_init(&t, CLOCK_REALTIME, 0, 1000000000 / 30);

	char buf[BUFSIZ];
	setbuf(stderr, buf);

	struct termios old_settings;

	set_single_key_input(&old_settings);

	char keybuf[5];
	char lk = last_key(keybuf, 5);
	while (space_objs_simulate(sos, lk, &c)) {
		canvas_print(&c, stderr);
		fflush(stderr);
		tick(&t);
		canvas_unprint(&c, stderr);
		lk = last_key(keybuf, 5);
	}

	CLEANUP:
	reset_single_key_input(&old_settings);
}

