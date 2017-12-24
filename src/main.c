#include "canvas.h"
#include "keys.h"
#include "space_obj.h"
#include "ticker.h"

int main(void)
{
	struct space_obj_type proj_type, player_type;
	sotype_init(&proj_type, 0);
		*sotype_icon(&proj_type) = pixel('`', RED);
		*sotype_lifetime(&proj_type) = 60;
	sotype_init(&player_type, SPACE_OBJ_PLAYER);
		*sotype_icon(&player_type) = pixel('X', GREEN);
		 sotype_icon(&player_type)->inverted = 1;
		*sotype_lifetime(&player_type) = -1;
		*sotype_health(&player_type) = 100;
		*sotype_reload(&player_type) = 10;
		*sotype_reload_burst(&player_type) = 100;
		*sotype_ammo(&player_type) = 10;
		*sotype_friction(&player_type) = 0.99;
		*sotype_acceleration(&player_type) = 0.03;
		*sotype_rotation(&player_type) = 0.05;
		projectile_init(sotype_proj(&player_type), &proj_type, 2.0, 2.0);
	struct space_obj_node sol;
	init_solist(&sol);
	struct space_obj *so = sonode_inner(&sol);
	space_obj_init(so, &player_type);
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
	while (simulate_solist(&sol, lk, &c)) {
		canvas_print(&c, stderr);
		fflush(stderr);
		tick(&t);
		canvas_unprint(&c, stderr);
		lk = last_key(keybuf, 5);
	}

	reset_single_key_input(&old_settings);
}

