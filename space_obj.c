#include "canvas.h"

#include <math.h>

typedef struct {
	float x, y;
} COORD;

struct space_obj {
	const struct space_obj_type *type;
	int health;
	int lifetime;
	int ammo, reload_burst;
	float direction;
	COORD pos;
	COORD vel;
};

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
};

void space_obj_update(struct space_obj *self)
{
	self->pos.x += self->vel.x;
	self->pos.y += self->vel.y;
	self->vel.x *= self->type->friction;
	self->vel.y *= self->type->friction;
}

void space_obj_rot(struct space_obj *self, float rotation)
{
	self->direction += rotation;
}

COORD space_obj_direction(const struct space_obj *self)
{
	COORD d;
	d.x = cosf(self->direction);
	d.y = sinf(self->direction);
	return d;
}

void space_obj_thrust(struct space_obj *self)
{
	COORD t = space_obj_direction(self);
	self->vel.x += t.x * self->type->acceleration;
	self->vel.y += t.y * self->type->acceleration;
}

void space_obj_draw(const struct space_obj *self, struct canvas *c)
{
	size_t x, y;
	if (self->pos.x < 0.0)
		return;
	else
		x = self->pos.x + 0.5;
	if (self->pos.y < 0.0)
		return;
	else
		y = self->pos.y / 2 + 0.5;

	PIXEL *at = canvas_get(c, x, y);
	if (at == NULL)
		return;
	else
		*at = self->type->icon;
}

#define EMPTY_SPACE_ICON (pixel(' ', WHITE))

void space_obj_undraw(const struct space_obj *self, struct canvas *c)
{
	size_t x, y;
	if (self->pos.x < 0.0)
		return;
	else
		x = self->pos.x + 0.5;
	if (self->pos.y < 0.0)
		return;
	else
		y = self->pos.y / 2.0 + 0.5;

	PIXEL *at = canvas_get(c, x, y);
	if (at == NULL)
		return;
	else
		*at = EMPTY_SPACE_ICON;
}

#include <unistd.h>

int main(void)
{
	struct space_obj_type sot;
	sot.icon = pixel('X', GREEN);
	invert_pixel(&sot.icon);
	sot.friction = 0.97;
	sot.acceleration = 0.02;
	struct space_obj so;
	so.type = &sot;
	so.pos.x = 0.0;
	so.vel.x = 0.0;
	so.pos.y = 50.0;
	so.vel.y = 0.0;
	struct canvas c;
	canvas_init(&c, 200, 50, EMPTY_SPACE_ICON);
	while (1) {
		space_obj_undraw(&so, &c);
		if (so.pos.y > 50.0)
			space_obj_rot(&so, -0.02);
		else
			space_obj_rot(&so, 0.02);
		space_obj_thrust(&so);
		space_obj_update(&so);
		space_obj_draw(&so, &c);
		canvas_print(&c, stdout);
		usleep(16666);
	}
}
