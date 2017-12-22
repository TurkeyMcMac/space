#ifndef _SPACE_OBJ_H

#define _SPACE_OBJ_H

#include "canvas.h"

typedef struct {
	float x, y;
} COORD;

struct space_obj {
	const struct space_obj_type *type;
	int health;
	int lifetime;
	int ammo, reload_burst;
	float angle;
	COORD dir;
	COORD pos;
	COORD vel;
};

void space_obj_update(struct space_obj *self);

void space_obj_rleft(struct space_obj *self);

void space_obj_rright(struct space_obj *self);

void space_obj_thrust(struct space_obj *self);

#define EMPTY_SPACE_ICON (pixel(' ', WHITE))

void space_obj_draw(struct space_obj *self, struct canvas *c);

void space_obj_undraw(struct space_obj *self, struct canvas *c);

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
	int reload, reload_burst;
	float mass;
       	float friction;
	float acceleration;
	float rotation;
};

SPACE_OBJ_FLAGS *sotype_flags(struct space_obj_type *self);

PIXEL *sotype_icon(struct space_obj_type *self);

const char **sotype_name(struct space_obj_type *self);

int *sotype_health(struct space_obj_type *self);

int *sotype_lifetime(struct space_obj_type *self);

int *sotype_reload(struct space_obj_type *self);

int *sotype_reload_burst(struct space_obj_type *self);

float *sotype_mass(struct space_obj_type *self);

float *sotype_friction(struct space_obj_type *self);

float *sotype_acceleration(struct space_obj_type *self);

float *sotype_rotation(struct space_obj_type *self);

#endif
