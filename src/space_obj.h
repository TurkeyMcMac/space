#ifndef _SPACE_OBJ_H

#define _SPACE_OBJ_H

#include "canvas.h"
#include "error.h"

#include <stdio.h>

typedef struct {
	float x, y;
} COORD;

struct space_obj {
	const struct space_obj_type *type;
	struct space_obj_node *target;
	int health;
	int lifetime;
	short reload, ammo;
	int has_rotated;
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

typedef unsigned char TEAM;

struct projectile {
	const struct space_obj_type *type;
	float distance;
	float velocity;
};

void projectile_init(struct projectile *p,
		const struct space_obj_type *type,
		float distance,
		float velocity);

struct space_obj_type {
	TEAM team, collide, target;
	char icon, color;
	const char *name;
	int health;
	int damage;
	int lifetime;
	short reload, reload_burst, ammo;
	float mass;
	float width;
	float friction;
	float acceleration;
	float rotation;
	struct projectile proj;
};

void sotype_init(struct space_obj_type *sot);

char *sotype_icon(struct space_obj_type *self);

char *sotype_color(struct space_obj_type *self);

const char **sotype_name(struct space_obj_type *self);

int *sotype_health(struct space_obj_type *self);

int *sotype_damage(struct space_obj_type *self);

int *sotype_lifetime(struct space_obj_type *self);

short *sotype_reload(struct space_obj_type *self);

short *sotype_reload_burst(struct space_obj_type *self);

short *sotype_ammo(struct space_obj_type *self);

float *sotype_mass(struct space_obj_type *self);

float *sotype_width(struct space_obj_type *self);

float *sotype_friction(struct space_obj_type *self);

float *sotype_acceleration(struct space_obj_type *self);

float *sotype_rotation(struct space_obj_type *self);

TEAM *sotype_team(struct space_obj_type *self);

TEAM *sotype_collide(struct space_obj_type *self);

TEAM *sotype_target(struct space_obj_type *self);

struct projectile *sotype_proj(struct space_obj_type *self);

void init_solist(struct space_obj_node *list);

struct space_obj *sonode_inner(struct space_obj_node *self);

void push_to_solist(struct space_obj_node *list, struct space_obj_node *p);

int simulate_solist(struct space_obj_node *list, char last_key, struct canvas *c);

int space_obj_print_stats(const struct space_obj *self, FILE *f);

int space_obj_unprint_stats(const struct space_obj *self, FILE *f);

void drop_solist(struct space_obj_node *list);

#endif
