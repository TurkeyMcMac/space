#include "space_obj.h"

#include "canvas.h"

#include <math.h>
#include <stdlib.h>

#define SPACE_OBJ_PLAYER (1 << 0)
#define SPACE_OBJ_TRACK (1 << 1)
#define SPACE_OBJ_SHOOT (1 << 2)
#define SPACE_OBJ_SOLID (1 << 3)
#define SPACE_OBJ_EFFECT (1 << 4)

struct simulated {
	enum sim_action action;
	struct space_obj_node *insert;
};

void space_obj_init(struct space_obj *so, const struct space_obj_type *type)
{
	so->type = type;
	so->health = type->health;
	so->lifetime = type->lifetime;
	so->ammo = type->reload;
	so->reload = 0;
	so->angle = 0.0;
	so->dir = (COORD) { 0.0, 0.0 };
	so->pos = (COORD) { 0.0, 0.0 };
	so->vel = (COORD) { 0.0, 0.0 };
}

static int space_obj_update(struct space_obj *self)
{
	self->pos.x += self->vel.x;
	self->pos.y += self->vel.y;
	self->vel.x *= self->type->friction;
	self->vel.y *= self->type->friction;
	if (self->lifetime == 0 || self->health <= 0)
		return REM_SELF;
	--self->lifetime;
	if (self->type->flags & (SPACE_OBJ_PLAYER | SPACE_OBJ_SHOOT)) {
		if (self->reload > 0)
			--self->reload;
		if (self->ammo == 0) {
			self->reload = self->type->reload_burst;
			self->ammo = self->type->ammo;
		}
	}
	return NOTHING;
}

static void space_obj_rotate(struct space_obj *self, float angle)
{
	self->angle += angle;
	self->dir = (COORD) { 0.0, 0.0 };
}

static void space_obj_calc_dir(struct space_obj *self)
{
	if (self->dir.x == 0.0 && self->dir.y == 0.0) {
		self->dir.x = cosf(self->angle);
		self->dir.y = sinf(self->angle);
	}
}

static void space_obj_rright(struct space_obj *self)
{
	space_obj_rotate(self, self->type->rotation);
}

static void space_obj_rleft(struct space_obj *self)
{
	space_obj_rotate(self, -self->type->rotation);
}

static void space_obj_thrust(struct space_obj *self)
{
	space_obj_calc_dir(self);
	COORD t = self->dir;
	self->vel.x += t.x * self->type->acceleration;
	self->vel.y += t.y * self->type->acceleration;
}

static PIXEL *canvas_get_float(struct canvas *c, COORD p)
{
	size_t x, y;
	if (p.x < 0.0)
		return NULL;
	else
		x = p.x + 0.5;
	if (p.y < 0.0)
		return NULL;
	else
		y = p.y / 2 + 0.5;
	return canvas_get(c, x, y);
}

static void space_obj_draw(struct space_obj *self, struct canvas *c)
{
	PIXEL *at = canvas_get_float(c, self->pos);
	if (at)
		*at = self->type->icon;
	if (self->type->flags & SPACE_OBJ_PLAYER) {
		space_obj_calc_dir(self);
		COORD targ = self->dir;
		targ.x *= 20.0;
		targ.y *= 20.0;
		targ.x += self->pos.x;
		targ.y += self->pos.y;
		at = canvas_get_float(c, targ);
		if (at == NULL)
			return;
		else
			at->inverted = 1;
	}
}

static void space_obj_undraw(struct space_obj *self, struct canvas *c)
{
	PIXEL *at = canvas_get_float(c, self->pos);
	if (at)
		*at = EMPTY_SPACE_ICON;
	if (self->type->flags & SPACE_OBJ_PLAYER) {
		space_obj_calc_dir(self);
		COORD targ = self->dir;
		targ.x *= 20.0;
		targ.y *= 20.0;
		targ.x += self->pos.x;
		targ.y += self->pos.y;
		at = canvas_get_float(c, targ);
		if (at == NULL)
			return;
		else
			at->inverted = 0;
	}
}
#define SO_GETTER(type, field) \
	type *space_obj_##field(struct space_obj *self) { return &self->field; }

SO_GETTER(int, health);
SO_GETTER(int, lifetime);
SO_GETTER(short, reload);
SO_GETTER(short, ammo);
SO_GETTER(float, angle);
SO_GETTER(COORD, dir);
SO_GETTER(COORD, pos);
SO_GETTER(COORD, vel);

void sotype_init(struct space_obj_type *sot, SPACE_OBJ_FLAGS flags)
{
	sot->flags = flags;
	sot->icon = pixel('_', WHITE);
	sot->name = "(none)";
	sot->health = 1;
	sot->lifetime = -1;
	sot->ammo = 0;
	sot->reload = 0;
	sot->reload_burst = 0;
	sot->mass = 1.0;
	sot->friction = 1.0;
	sot->acceleration = 0.0;
	sot->rotation = 0.0;
}

#define SOTYPE_GETTER(type, field) \
	type *sotype_##field(struct space_obj_type *self) { return &self->field; }

SOTYPE_GETTER(PIXEL, icon);
SOTYPE_GETTER(const char *, name);
SOTYPE_GETTER(int, health);
SOTYPE_GETTER(int, lifetime);
SOTYPE_GETTER(short, reload);
SOTYPE_GETTER(short, reload_burst);
SOTYPE_GETTER(short, ammo);
SOTYPE_GETTER(float, mass);
SOTYPE_GETTER(float, friction);
SOTYPE_GETTER(float, acceleration);
SOTYPE_GETTER(float, rotation);

static struct space_obj_node *space_obj_shoot(struct space_obj *self)
{
	static struct space_obj_type bullet_ty = { .name = NULL };
	if (bullet_ty.name == NULL) {
		sotype_init(&bullet_ty, 0);
		*sotype_icon(&bullet_ty) = pixel('`', RED);
		*sotype_name(&bullet_ty) = "bullet";
		*sotype_lifetime(&bullet_ty) = 60;
	}
	if (self->reload == 0) {
		self->reload = self->type->reload;
		--self->ammo;
		struct space_obj_node *b = malloc(sizeof(struct space_obj_node));
		space_obj_init(&b->so, &bullet_ty);
		b->so.pos = self->pos;
		space_obj_calc_dir(self);
		b->so.pos.x += self->dir.x * 2;
		b->so.pos.y += self->dir.y * 2;
		b->so.vel = self->vel;
		b->so.vel.x += self->dir.x * 2;
		b->so.vel.y += self->dir.y * 2;
		return b;
	} else
		return NULL;

}

static void space_obj_simulate(struct space_obj *self, /* TODO: Remove some arguments */
		struct space_obj_node *others,
		char last_key,
		struct simulated *result,
		struct canvas *c)
{
	space_obj_undraw(self, c);
	if (self->type->flags & SPACE_OBJ_PLAYER) {
		switch (last_key) {
			case 'w':
				space_obj_thrust(self);
				break;
			case 'a':
				space_obj_rleft(self);
				break;
			case 'd':
				space_obj_rright(self);
				break;
			case 'q':
				space_obj_rleft(self);
				space_obj_thrust(self);
				break;
			case 'e':
				space_obj_rright(self);
				space_obj_thrust(self);
				break;
			case 'W':
				space_obj_thrust(self);
				result->insert = space_obj_shoot(self);
				goto UPDATE;
			case 'A':
				space_obj_rleft(self);
				result->insert = space_obj_shoot(self);
				goto UPDATE;
			case 'D':
				space_obj_rright(self);
				result->insert = space_obj_shoot(self);
				goto UPDATE;
			case 'Q':
				space_obj_rleft(self);
				space_obj_thrust(self);
				result->insert = space_obj_shoot(self);
				goto UPDATE;
			case 'E':
				space_obj_rright(self);
				space_obj_thrust(self);
				result->insert = space_obj_shoot(self);
				goto UPDATE;
			case ' ':
				result->insert = space_obj_shoot(self);
				goto UPDATE;
			case '\04':
				result->action = STOP_GAME;
				return;
		}
	}
	result->insert = NULL;
	UPDATE:
	result->action = space_obj_update(self);
	if (result->action != REM_SELF)
		space_obj_draw(self, c);
}

int space_objs_simulate(struct space_obj_node *list, char last_key, struct canvas *c)
{
	struct space_obj_node *node, *last_node;
	for (node = list; node != NULL; last_node = node, node = node->next) {
		struct simulated simmed;
		space_obj_simulate(&node->so, list, last_key, &simmed, c);
		switch (simmed.action) {
			case STOP_GAME:
				return 0;
			case REM_SELF: /* The first item (the player) will never be removed, so
					  that case need not be handled. */
				last_node->next = node->next;
				free(node);
				node = last_node;
				break;
		}
		if (simmed.insert) {
			simmed.insert->next = node->next;
			node->next = simmed.insert;
		}
	}
	return 1;
}
