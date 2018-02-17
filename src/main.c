#include "canvas.h"
#include "cmdopt.h"
#include "error.h"
#include "keys.h"
#include "space_obj.h"
#include "ticker.h"

#include <errno.h>
#include <signal.h>
#include <stdlib.h>

struct game_settings {
	size_t width, height;
	long fps;
};

int cancelled = 0;

void cancel_game(int _)
{
	cancelled = 1;
}

void init_sotypes(struct space_obj_type *proj_type,
		struct space_obj_type *player_type,
		struct space_obj_type *factory_type,
		struct space_obj_type *npc_type,
		struct space_obj_type *drone_type);


#define DECLARE_CMDOPT(name, description...) \
const char * const name##_description[] = { description, NULL }; \
int name##_handler(const struct cmdopt *self, const char *arg, void *env);

#define DEFINE_FIELD_SETTER(field, no_arg_error) \
int field##_handler(const struct cmdopt *self, const char *arg, void *settings) \
{ \
	if (arg == NULL) { \
		fprintf(stderr, no_arg_error "\n"); \
		return -1; \
	} else { \
		((struct game_settings *)settings)->field = atol(arg); \
		return 0; \
	} \
}

DECLARE_CMDOPT(fps,
	"Set frames per second to the argument provided.",
	"Default value: 20.");
DECLARE_CMDOPT(height,
	"Set world height to the argument provided.",
	"Default value: 40.");
DECLARE_CMDOPT(help,
	"Display help information.");
DECLARE_CMDOPT(width,
	"Set world width to the argument provided.",
	"Default value: 150.");

#define N_COPT 4
const struct cmdopt opts[N_COPT] = {
	{ 'f', "frames-per-second", fps_description,    fps_handler     },
	{ 'H', "height",            height_description, height_handler  },
	{ 'h', "help",              help_description,   help_handler    },
	{ 'W', "width",             width_description,  width_handler   },
};

#define HELP_PADDING 30
int help_handler(const struct cmdopt *self, const char *arg, void *env)
{
	if CATCH (printf,("Usage: space [-(O...|-OPTION)[=ARGUMENT]]...\n"
			  "Run the game 'space'.\n"
			  "Options:\n"))
		print_errs(stderr);
	size_t i;
	for (i = 0; i < N_COPT; ++i) {
		if CATCH (print_cmdopt,(&opts[i], HELP_PADDING, stdout))
			print_errs(stderr);
	}
	return 1;
}

DEFINE_FIELD_SETTER(fps, "How many frames per second do you want?");
DEFINE_FIELD_SETTER(width, "How wide should the world be?");
DEFINE_FIELD_SETTER(height, "How tall should the world be?");

int err_handler(size_t len, const char *bad, void *env)
{
	fprintf(stderr, "Invalid option: '%.*s'.\n"
			"Provide the option '-h' or '--help' for help "
			"information.\n", (int)len, bad);
	return -1;
}

int main(int argc, char *argv[])
{
	int errnum = 0;
	size_t i;

	struct sigaction canceller;
	canceller.sa_handler = cancel_game;
	if CATCH (sigaction,(SIGINT, &canceller, NULL))
		print_errs(stderr);

	struct game_settings settings = {
		.width = 150,
		.height = 40,
		.fps = 20,
	};
	struct cmdopt_parser parser;
	cmdopt_parser_init(&parser, N_COPT, opts);
	for (i = 1; i < argc; ++i) {
		if (parse_cmdopts(&parser, argv[i], err_handler, &settings) != 0)
			return 0;
	}

	struct space_obj_type proj_type, player_type, npc_type, factory_type, drone_type;
	init_sotypes(&proj_type, &player_type, &factory_type, &npc_type, &drone_type);

	struct space_obj_node sol;
	init_solist(&sol);
	struct space_obj *so = sonode_inner(&sol);
	space_obj_init(so, &player_type);
	space_obj_pos(so)->y = 50.0;

	for (i = 0; i < 2; ++i) {
		struct space_obj_node *factory_node = malloc(sizeof(struct space_obj_node));
		init_solist(factory_node);
		space_obj_init(sonode_inner(factory_node), &factory_type);
		sonode_inner(factory_node)->pos = (COORD) { 50.0, 50.0 };
		push_to_solist(&sol, factory_node);
	}

	struct canvas c;
	canvas_init(&c, settings.width, settings.height, EMPTY_SPACE_ICON);

	struct ticker t;
	if CATCH (ticker_init,(&t, CLOCK_MONOTONIC, 1e9 / settings.fps)) {
		errnum = errno;
		print_errs(stderr);
		return errnum;
	}

	char buf[BUFSIZ];
	setbuf(stdout, buf);

	struct terminal_info term_info;

	if CATCH (set_single_key_input,(&term_info)) {
		errnum = errno;
		print_errs(stderr);
		return errnum;
	}

	char keybuf[5];
	char lk;
	while (!cancelled && simulate_solist(&sol, lk, &c)) {
		if CATCH_TO (lk, last_key,(keybuf, 5, &term_info)) {
			lk = '\0';
			print_errs(stderr);
		}
		if (CATCH (canvas_print,(&c, stdout))
		 || CATCH (space_obj_print_stats,(sonode_inner(&sol), stdout))
		 || CATCH (fflush,(stdout)))
			print_errs(stderr);
		if CATCH (tick,(&t))
			print_errs(stderr);
		if (CATCH (space_obj_unprint_stats,(sonode_inner(&sol), stdout))
		 || CATCH (canvas_unprint,(&c, stdout)))
			print_errs(stderr);
	}

	if CATCH (printf,("Game over.\n"))
		print_errs(stderr);
	
	drop_solist(&sol);
	canvas_drop(&c);

	if CATCH (reset_single_key_input,(&term_info)) {
		errnum = errno;
		print_errs(stderr);
	}

	drop_err_buf();

	return errnum;
}

void init_sotypes(struct space_obj_type *proj_type,
		struct space_obj_type *player_type,
		struct space_obj_type *factory_type,
		struct space_obj_type *npc_type,
		struct space_obj_type *drone_type)
{
	sotype_init(proj_type);
		*sotype_name(proj_type) = "Projectile";
		*sotype_icon(proj_type) = '`';
		*sotype_color(proj_type) = MAGENTA;
		*sotype_lifetime(proj_type) = 100;
		*sotype_team(proj_type) = 1 << 1;
		*sotype_collide(proj_type) = ~(1 << 1);
		*sotype_width(proj_type) = 0.1;
	sotype_init(player_type);
		*sotype_name(player_type) = "Player";
		*sotype_icon(player_type) = 'X';
		*sotype_color(player_type) = BLUE;
		*sotype_lifetime(player_type) = -1;
		*sotype_health(player_type) = 15;
		*sotype_reload(player_type) = 20;
		*sotype_reload_burst(player_type) = 50;
		*sotype_ammo(player_type) = 2;
		*sotype_friction(player_type) = 0.99;
		*sotype_mass(player_type) = 20.0;
		*sotype_acceleration(player_type) = 0.01;
		*sotype_rotation(player_type) = 0.02;
		*sotype_target(player_type) = ~1;
		projectile_init(sotype_proj(player_type), drone_type, -3.0, -0.4);
	sotype_init(factory_type);
		*sotype_name(factory_type) = "Factory";
		*sotype_team(factory_type) = 1 << 1;
		*sotype_collide(factory_type) = ~(1 << 1);
		*sotype_width(factory_type) = 1.0;
		*sotype_target(factory_type) = ~(1 << 1);
		*sotype_icon(factory_type) = '#';
		*sotype_color(factory_type) = RED;
		*sotype_lifetime(factory_type) = -1;
		*sotype_health(factory_type) = 10;
		*sotype_friction(factory_type) = 0.99;
		*sotype_mass(factory_type) = 200.0;
		*sotype_rotation(factory_type) = 0.05;
		*sotype_reload(factory_type) = 0;
		*sotype_reload_burst(factory_type) = 500;
		*sotype_ammo(factory_type) = 1;
		projectile_init(sotype_proj(factory_type), npc_type, 0.7, 0.1);
	sotype_init(npc_type);
		*sotype_name(npc_type) = "Enemy";
		*sotype_team(npc_type) = 1 << 1;
		*sotype_collide(npc_type) = ~(1 << 1);
		*sotype_target(npc_type) = ~(1 << 1);
		*sotype_icon(npc_type) = '@';
		*sotype_color(npc_type) = RED;
		*sotype_lifetime(npc_type) = -1;
		*sotype_health(npc_type) = 20;
		*sotype_friction(npc_type) = 0.99;
		*sotype_mass(npc_type) = 20.0;
		*sotype_acceleration(npc_type) = 0.015;
		*sotype_rotation(npc_type) = 0.02;
		*sotype_reload(npc_type) = 20;
		*sotype_reload_burst(npc_type) = 20;
		*sotype_ammo(npc_type) = 10;
		projectile_init(sotype_proj(npc_type), proj_type, 3.0, 1.0);
	sotype_init(drone_type);
		*sotype_name(drone_type) = "Missile";
		*sotype_icon(drone_type) = '*';
		*sotype_color(drone_type) = CYAN;
		*sotype_lifetime(drone_type) = 400;
		*sotype_health(drone_type) = 1;
		*sotype_damage(drone_type) = 5;
		*sotype_friction(drone_type) = 0.995;
		*sotype_mass(drone_type) = 3.0;
		*sotype_acceleration(drone_type) = 0.020;
		*sotype_rotation(drone_type) = 0.07;
}
