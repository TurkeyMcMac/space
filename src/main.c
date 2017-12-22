#include "space_obj.h"
#include "canvas.h"

#include <unistd.h>

int main(void)
{
	struct space_obj_type sot;
	sot.icon = pixel('X', GREEN);
	invert_pixel(&sot.icon);
	sot.friction = 0.96;
	sot.acceleration = 0.02;
	sot.rotation = 0.02;
	struct space_obj so;
	so.type = &sot;
	so.direction = 0.0;
	so.pos.x = 0.0;
	so.vel.x = 0.0;
	so.pos.y = 50.0;
	so.vel.y = 0.0;
	struct canvas c;
	canvas_init(&c, 200, 50, EMPTY_SPACE_ICON);
	while (1) {
		space_obj_undraw(&so, &c);
		if (so.pos.y > 50.0)
			space_obj_rleft(&so);
		else
			space_obj_rright(&so);
		space_obj_thrust(&so);
		space_obj_update(&so);
		space_obj_draw(&so, &c);
		canvas_print(&c, stdout);
		usleep(16666);
	}
}

