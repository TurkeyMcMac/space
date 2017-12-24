#ifndef _SPACE_OBJ_H

#define _SPACE_OBJ_H

#include "canvas.h"

typedef struct {
	float x, y;
} COORD;

struct space_obj {
	const struct space_obj_type *type;
	struct space_obj_node *target;
	int health;
	int lifetime;
	short reload, ammo;
	float angle;
	COORD dir;
	COORD pos;
	COORD vel;
};

struct space_obj_node {
	struct space_obj_node *next;
	size_t rc;
	struct space_obj so;
};

void space_obj_init(struct space_obj *so, const struct space_obj_type *type);

enum sim_action {
	NOTHING,
	REM_SELF,
	STOP_GAME,
};

#define EMPTY_SPACE_ICON (pixel(' ', WHITE))

int *space_obj_health(struct space_obj *self);

int *space_obj_lifetime(struct space_obj *self);

short *space_obj_ammo(struct space_obj *self);

short *space_obj_reload(struct space_obj *self);

short *space_obj_ammo(struct space_obj *self);

float *space_obj_angle(struct space_obj *self);

COORD *space_obj_dir(struct space_obj *self);

COORD *space_obj_pos(struct space_obj *self);

COORD *space_obj_vel(struct space_obj *self);

#define SPACE_OBJ_PLAYER (1 << 0)
#define SPACE_OBJ_TRACK (1 << 1)
#define SPACE_OBJ_SHOOT (1 << 2)
#define SPACE_OBJ_SOLID (1 << 3)
#define SPACE_OBJ_EFFECT (1 << 4)

typedef int SPACE_OBJ_FLAGS;

struct space_obj_type {
	SPACE_OBJ_FLAGS flags;
	PIXEL icon;
	const char *name;
	int health;
	int lifetime;
	short reload, reload_burst, ammo;
	float mass;
       	float friction;
	float acceleration;
	float rotation;
};

void sotype_init(struct space_obj_type *sot, SPACE_OBJ_FLAGS flags);

PIXEL *sotype_icon(struct space_obj_type *self);

const char **sotype_name(struct space_obj_type *self);

int *sotype_health(struct space_obj_type *self);

int *sotype_lifetime(struct space_obj_type *self);

short *sotype_reload(struct space_obj_type *self);

short *sotype_reload_burst(struct space_obj_type *self);

short *sotype_ammo(struct space_obj_type *self);

float *sotype_mass(struct space_obj_type *self);

float *sotype_friction(struct space_obj_type *self);

float *sotype_acceleration(struct space_obj_type *self);

float *sotype_rotation(struct space_obj_type *self);

int simulate_solist(struct space_obj_node *list, char last_key, struct canvas *c);

#endif
