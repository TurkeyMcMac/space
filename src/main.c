#include "canvas.h"
#include "keys.h"
#include "space_obj.h"
#include "ticker.h"

int main(void)
{
	struct space_obj_type proj_type, player_type, npc_type;
	sotype_init(&proj_type, SPACE_OBJ_SOLID);
		*sotype_icon(&proj_type) = pixel('`', RED);
		*sotype_lifetime(&proj_type) = 100;
	sotype_init(&player_type, SPACE_OBJ_PLAYER | SPACE_OBJ_SOLID);
		*sotype_icon(&player_type) = pixel('X', GREEN);
		 sotype_icon(&player_type)->inverted = 1;
		*sotype_lifetime(&player_type) = -1;
		*sotype_health(&player_type) = 100;
		*sotype_reload(&player_type) = 10;
		*sotype_reload_burst(&player_type) = 20;
		*sotype_ammo(&player_type) = 10;
		*sotype_friction(&player_type) = 0.99;
		*sotype_acceleration(&player_type) = 0.01;
		*sotype_rotation(&player_type) = 0.03;
		projectile_init(sotype_proj(&player_type), &proj_type, 0.7, 0.7);
	sotype_init(&npc_type, SPACE_OBJ_TRACK | SPACE_OBJ_SHOOT | SPACE_OBJ_SOLID);
		*sotype_icon(&npc_type) = pixel('X', YELLOW);
		*sotype_lifetime(&npc_type) = -1;
		*sotype_health(&npc_type) = 100;
		*sotype_friction(&npc_type) = 0.99;
		*sotype_acceleration(&npc_type) = 0.007;
		*sotype_rotation(&npc_type) = 0.02;
		*sotype_reload(&npc_type) = 20;
		*sotype_reload_burst(&npc_type) = 20;
		*sotype_ammo(&npc_type) = 10;
		projectile_init(sotype_proj(&npc_type), &proj_type, 0.1, 0.7);
	struct space_obj_node sol, *npc_node1, *npc_node2;
	npc_node1 = malloc(sizeof(struct space_obj_node));
	npc_node2 = malloc(sizeof(struct space_obj_node));
	init_solist(&sol);
	init_solist(npc_node1);
	init_solist(npc_node2);
	struct space_obj *so = sonode_inner(&sol);
	space_obj_init(so, &player_type);
	space_obj_pos(so)->y = 50.0;

	space_obj_init(sonode_inner(npc_node1), &npc_type);
	sonode_inner(npc_node1)->target = &sol;

	space_obj_init(sonode_inner(npc_node2), &npc_type);
	sonode_inner(npc_node2)->target = &sol;
	sonode_inner(npc_node2)->pos = (COORD) { 48.0, 75.0 };
	sonode_inner(npc_node2)->vel = (COORD) { 1.1, -0.3 };
	push_to_solist(&sol, npc_node1);
	push_to_solist(&sol, npc_node2);
	struct canvas c;
	canvas_init(&c, 200, 50, EMPTY_SPACE_ICON);

	struct ticker t;
	ticker_init(&t, CLOCK_REALTIME, 0, 1000000000 / 40);

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

