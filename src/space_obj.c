#include "space_obj.h"

#include "canvas.h"

#include <math.h>

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
