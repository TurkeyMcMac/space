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

static void space_obj_rotate(struct space_obj *self, float angle)
{
	float sine, cosine;
	sine = sinf(angle);
	cosine = cosf(angle);
	self->dir.x = self->dir.x * cosine - self->dir.y * sine;
	self->dir.y = self->dir.x * sine + self->dir.y * cosine;
}

void space_obj_rright(struct space_obj *self)
{
	space_obj_rotate(self, self->type->rotation);
}

void space_obj_rleft(struct space_obj *self)
{
	space_obj_rotate(self, -self->type->rotation);
}

void space_obj_thrust(struct space_obj *self)
{
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

void space_obj_draw(const struct space_obj *self, struct canvas *c)
{
	PIXEL *at = canvas_get_float(c, self->pos);
	if (at == NULL)
		return;
	else
		*at = self->type->icon;
	if (self->type->flags & SPACE_OBJ_PLAYER) {
		COORD targ = self->dir;
		targ.x *= 20.0;
		targ.y *= 20.0;
		targ.x += self->pos.x;
		targ.y += self->pos.y;
		at = canvas_get_float(c, targ);
		if (at == NULL)
			return;
		else
			invert_pixel(at);
	}
}

void space_obj_undraw(const struct space_obj *self, struct canvas *c)
{
	PIXEL *at = canvas_get_float(c, self->pos);
	if (at == NULL)
		return;
	else
		*at = EMPTY_SPACE_ICON;
	if (self->type->flags & SPACE_OBJ_PLAYER) {
		COORD targ = self->dir;
		targ.x *= 20.0;
		targ.y *= 20.0;
		targ.x += self->pos.x;
		targ.y += self->pos.y;
		at = canvas_get_float(c, targ);
		if (at == NULL)
			return;
		else
			invert_pixel(at);
	}
}
