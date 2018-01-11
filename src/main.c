#include "canvas.h"
#include "error.h"
#include "keys.h"
#include "space_obj.h"
#include "ticker.h"

#include <errno.h>
#include <stdlib.h>

int main(void)
{
	struct space_obj_type proj_type, player_type, npc_type, drone_type;
	sotype_init(&proj_type, 0);
		*sotype_name(&proj_type) = "Projectile";
		*sotype_icon(&proj_type) = '`';
		*sotype_color(&proj_type) = YELLOW;
		*sotype_lifetime(&proj_type) = 100;
		*sotype_team(&proj_type) = 1 << 1;
		*sotype_collide(&proj_type) = ~(1 << 1);
		*sotype_width(&proj_type) = 0.1;
	sotype_init(&player_type, SPACE_OBJ_PLAYER);
		*sotype_name(&player_type) = "Player";
		*sotype_icon(&player_type) = 'X';
		*sotype_color(&player_type) = GREEN;
		*sotype_lifetime(&player_type) = -1;
		*sotype_health(&player_type) = 10;
		*sotype_reload(&player_type) = 20;
		*sotype_reload_burst(&player_type) = 50;
		*sotype_ammo(&player_type) = 2;
		*sotype_friction(&player_type) = 0.99;
		*sotype_mass(&player_type) = 20.0;
		*sotype_acceleration(&player_type) = 0.01;
		*sotype_rotation(&player_type) = 0.03;
		*sotype_target(&player_type) = ~1;
		projectile_init(sotype_proj(&player_type), &drone_type, 3.0, 0.4);
	sotype_init(&npc_type, SPACE_OBJ_SHOOT | SPACE_OBJ_TRACK);
		*sotype_name(&npc_type) = "Enemy";
		*sotype_team(&npc_type) = 1 << 1;
		*sotype_collide(&npc_type) = ~(1 << 1);
		*sotype_target(&npc_type) = ~0;
		*sotype_icon(&npc_type) = 'X';
		*sotype_color(&npc_type) = YELLOW;
		*sotype_lifetime(&npc_type) = -1;
		*sotype_health(&npc_type) = 10;
		*sotype_friction(&npc_type) = 0.99;
		*sotype_mass(&npc_type) = 20.0;
		*sotype_acceleration(&npc_type) = 0.007;
		*sotype_rotation(&npc_type) = 0.02;
		*sotype_reload(&npc_type) = 20;
		*sotype_reload_burst(&npc_type) = 20;
		*sotype_ammo(&npc_type) = 10;
		projectile_init(sotype_proj(&npc_type), &proj_type, 3.0, 1.0);
	sotype_init(&drone_type, SPACE_OBJ_TRACK);
		*sotype_name(&drone_type) = "Missile";
		*sotype_icon(&drone_type) = '*';
		*sotype_color(&drone_type) = GREEN;
		*sotype_lifetime(&drone_type) = 400;
		*sotype_health(&drone_type) = 1;
		*sotype_damage(&drone_type) = 2;
		*sotype_friction(&drone_type) = 0.995;
		*sotype_mass(&drone_type) = 3.0;
		*sotype_acceleration(&drone_type) = 0.020;
		*sotype_rotation(&drone_type) = 0.07;
		*sotype_reload(&drone_type) = 30;
		*sotype_reload_burst(&drone_type) = 40;
		*sotype_ammo(&drone_type) = 10;
		*sotype_width(&drone_type) = 0.2;
		projectile_init(sotype_proj(&drone_type), &proj_type, 3.0, 0.7);
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

	space_obj_init(sonode_inner(npc_node2), &npc_type);
	sonode_inner(npc_node2)->pos = (COORD) { 48.0, 75.0 };
	sonode_inner(npc_node2)->target = &sol;
	push_to_solist(&sol, npc_node1);
	push_to_solist(&sol, npc_node2);
	struct canvas c;
	canvas_init(&c, 200, 50, EMPTY_SPACE_ICON);

	struct ticker t;
	if CATCH (ticker_init,(&t, CLOCK_REALTIME, 0, 1000000000 / 40)) {
		int errnum = errno;
		print_errs(stderr);
		return errnum;
	}

	char buf[BUFSIZ];
	setbuf(stdout, buf);

	struct termios old_settings;

	if CATCH (set_single_key_input,(&old_settings)) {
		int errnum = errno;
		print_errs(stderr);
		return errnum;
	}

	char keybuf[5];
	char lk;
	while (simulate_solist(&sol, lk, &c)) {
		if CATCH_TO (lk, last_key,(keybuf, 5)) {
			lk = '\0';
			print_errs(stderr);
		}
		if CATCH (canvas_print,(&c, stdout))
			print_errs(stderr);
		if CATCH (fflush,(stdout))
			print_errs(stderr);
		if CATCH (tick,(&t))
			print_errs(stderr);
		if CATCH (canvas_unprint,(&c, stdout))
			print_errs(stderr);
	}

	if CATCH (reset_single_key_input,(&old_settings)) {
		int errnum = errno;
		print_errs(stderr);
		return errnum;
	} else
		return 0;
}

