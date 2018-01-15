#include "space_obj.h"

#include "canvas.h"
#include "error.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define so_projectile(so_ptr) ((so_ptr)->type->proj.type)

void space_obj_init(struct space_obj *so, const struct space_obj_type *type)
{
	so->type = type;
	so->target = NULL;
	so->health = type->health;
	so->lifetime = type->lifetime;
	so->ammo = 0;
	so->reload = 0;
	so->angle = 0.0;
	so->dir = (COORD) { 1.0, 0.0 };
	so->pos = (COORD) { 0.0, 0.0 };
	so->vel = (COORD) { 0.0, 0.0 };
}

static int space_obj_death(struct space_obj *self)
{
	return self->lifetime == 0 || self->health <= 0;
}

#define WORLD_WIDTH 200
#define WORLD_HEIGHT 100
#define WALL_BOUNCE_REDUCTION 0.5

static void space_obj_move(struct space_obj *self, float world_width, float world_height)
{
	if (self->pos.x < self->type->width) {
		self->pos.x = self->type->width;
		self->vel.x *= -WALL_BOUNCE_REDUCTION;
	} else if (self->pos.x > world_width - self->type->width) {
		self->pos.x = world_width - self->type->width;
		self->vel.x *= -WALL_BOUNCE_REDUCTION;
	} else
		self->pos.x += self->vel.x;
	if (self->pos.y < self->type->width) {
		self->pos.y = self->type->width;
		self->vel.y *= -WALL_BOUNCE_REDUCTION;
	} else if (self->pos.y > world_height - self->type->width) {
		self->pos.y = world_height - self->type->width;
		self->vel.y *= -WALL_BOUNCE_REDUCTION;
	} else
		self->pos.y += self->vel.y;
	self->vel.x *= self->type->friction;
	self->vel.y *= self->type->friction;
}

static void space_obj_do_reload(struct space_obj *self)
{
	if (self->reload > 0)
		--self->reload;
	if (self->ammo == 0) {
		self->reload = self->type->reload_burst;
		self->ammo = self->type->ammo;
	}
}

static void space_obj_rotate(struct space_obj *self, float angle)
{
	self->angle += angle;
	self->dir.x = FP_NAN;
}

static void space_obj_calc_dir(struct space_obj *self)
{
	if (self->dir.x == FP_NAN) {
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
	if (at) {
		at->ch = self->type->icon;
		at->color = self->type->color;
	}
}

static void space_obj_draw_player(struct space_obj *self, struct canvas *c)
{
	space_obj_draw(self, c);
	space_obj_calc_dir(self);
	COORD targ = self->dir;
	targ.x *= 20.0;
	targ.y *= 20.0;
	targ.x += self->pos.x;
	targ.y += self->pos.y;
	PIXEL *at = canvas_get_float(c, targ);
	if (at == NULL)
		return;
	else
		at->inverted = 1;
}

static void space_obj_undraw(struct space_obj *self, struct canvas *c)
{
	PIXEL *at = canvas_get_float(c, self->pos);
	if (at) {
		at->ch = ' ';
		at->color = WHITE;
	}
}

static void space_obj_undraw_player(struct space_obj *self, struct canvas *c)
{
	space_obj_undraw(self, c);
	space_obj_calc_dir(self);
	COORD targ = self->dir;
	targ.x *= 20.0;
	targ.y *= 20.0;
	targ.x += self->pos.x;
	targ.y += self->pos.y;
	PIXEL *at = canvas_get_float(c, targ);
	if (at == NULL)
		return;
	else
		at->inverted = 0;
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

void projectile_init(struct projectile *p,
		const struct space_obj_type *type,
		float distance,
		float velocity)
{
	p->type = type;
	p->distance = distance;
	p->velocity = velocity;
}

void sotype_init(struct space_obj_type *sot)
{
	sot->team = 1;
	sot->target = 0;
	sot->collide = ~1;
	sot->icon = '_';
	sot->color = WHITE;
	sot->name = "(none)";
	sot->health = 1;
	sot->damage = 1;
	sot->lifetime = -1;
	sot->ammo = 0;
	sot->reload = 0;
	sot->reload_burst = 0;
	sot->mass = 1.0;
	sot->width = 0.75;
	sot->friction = 1.0;
	sot->acceleration = 0.0;
	sot->rotation = 0.0;
	sot->proj = (struct projectile) {
		.type = NULL,
		.distance = 0.0,
		.velocity = 0.0,
	};
}

#define SOTYPE_GETTER(type, field) \
	type *sotype_##field(struct space_obj_type *self) { return &self->field; }

SOTYPE_GETTER(char, icon);
SOTYPE_GETTER(char, color);
SOTYPE_GETTER(const char *, name);
SOTYPE_GETTER(int, health);
SOTYPE_GETTER(int, lifetime);
SOTYPE_GETTER(short, reload);
SOTYPE_GETTER(short, reload_burst);
SOTYPE_GETTER(short, ammo);
SOTYPE_GETTER(float, mass);
SOTYPE_GETTER(float, width);
SOTYPE_GETTER(float, friction);
SOTYPE_GETTER(float, acceleration);
SOTYPE_GETTER(float, rotation);
SOTYPE_GETTER(struct projectile, proj);
SOTYPE_GETTER(TEAM, team);
SOTYPE_GETTER(TEAM, collide);
SOTYPE_GETTER(TEAM, target);
SOTYPE_GETTER(int, damage);

static struct space_obj_node *space_obj_shoot(struct space_obj *self)
{
	if (self->reload == 0) {
		self->reload = self->type->reload;
		--self->ammo;
		struct space_obj_node *p = malloc(sizeof(struct space_obj_node));
		const struct projectile *proj = &self->type->proj;
		space_obj_init(&p->so, proj->type);
		if (self->target != NULL) {
			p->so.target = self->target;
			++self->target->rc;
		}
		p->so.pos = self->pos;
		space_obj_calc_dir(self);
		p->so.pos.x += self->dir.x * proj->distance;
		p->so.pos.y += self->dir.y * proj->distance;
		p->so.vel = self->vel;
		p->so.vel.x += self->dir.x * proj->velocity;
		p->so.vel.y += self->dir.y * proj->velocity;
		return p;
	} else
		return NULL;

}

#define NODE_UNLINKED ((struct space_obj_node *)~0)

static void sonode_drop(struct space_obj_node *self)
{
	if (--self->rc == 0 && self->next == NODE_UNLINKED)
		free(self);
}

static struct space_obj *sonode_get(struct space_obj_node *self)
{
	if (self->next == NODE_UNLINKED) {
		if (--self->rc == 0)
			free(self);
		return NULL;
	} else
		return &self->so;
}

static void space_obj_collide(struct space_obj *self, struct space_obj *other)
{
	COORD diff;
	diff.x = other->pos.x - self->pos.x;
	diff.y = other->pos.y - self->pos.y;
	if (fabsf(diff.x) < self->type->width * 2.0 && fabsf(diff.y) < self->type->width * 2.0) {
		if (self->type->collide & other->type->team) {
			self->health -= other->type->damage;
			other->health -= self->type->damage;
		}
	

		if (fabsf(diff.x) > fabsf(diff.y)) {
			if (diff.x > 0.0)
				self->pos.x = other->pos.x - self->type->width * 2.0;
			else
				self->pos.x = other->pos.x + self->type->width * 2.0;
			self->vel.x = (self->vel.x * self->type->mass + other->vel.x * other->type->mass) /
				(self->type->mass + other->type->mass);
			other->vel.x = self->vel.x;
		} else {
			if (diff.y > 0.0)
				self->pos.y = other->pos.y - self->type->width * 2.0;
			else
				self->pos.y = other->pos.y + self->type->width * 2.0;
			self->vel.y = (self->vel.y * self->type->mass + other->vel.y * other->type->mass) /
				(self->type->mass + other->type->mass);
			other->vel.y = self->vel.y;
		}
	}
}

static void space_obj_collisions(struct space_obj *self, struct space_obj_node *others)
{
	for (; others != NULL; others = others->next)
		if (self != &others->so)
			space_obj_collide(self, &others->so);
}

static struct space_obj_node *space_obj_react_player(struct space_obj *self,
		struct space_obj_node *others,
		char last_key)
{
	space_obj_collisions(self, others);
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
			return space_obj_shoot(self);
		case 'A':
			space_obj_rleft(self);
			return space_obj_shoot(self);
		case 'D':
			space_obj_rright(self);
			return space_obj_shoot(self);
		case 'Q':
			space_obj_rleft(self);
			space_obj_thrust(self);
			return space_obj_shoot(self);
		case 'E':
			space_obj_rright(self);
			space_obj_thrust(self);
			return space_obj_shoot(self);
		case ' ':
			return space_obj_shoot(self);
		case '\04':
			self->health = 0;
			break;
	}
	return NULL;
}

static struct space_obj_node *space_obj_react(struct space_obj *self, struct space_obj_node *others)
{
	struct space_obj_node *ret = NULL;
	if (self->type->acceleration > 0.0 || so_projectile(self) != NULL) {
		struct space_obj_node *target;
		if (self->target == NULL || sonode_get(self->target) == NULL) {
			self->target = NULL;
			float target_dist;
			for (target_dist = 999999999999.0; others != NULL; others = others->next) {
				struct space_obj *other = &others->so;
				if (self == other)
					continue;
				space_obj_collide(self, other);
				if (self->type->team & other->type->target) {
					float other_dist = fabsf(other->pos.x - self->pos.x)
						+ fabsf(other->pos.y - self->pos.y);
					if (other_dist < target_dist) {
						target_dist = other_dist;
						self->target = others;
					}
				}
			}
			if (self->target != NULL) {
				++self->target->rc;
			}
			return NULL;
		} else {
			COORD tvec;
			tvec.x = self->target->so.pos.x - self->pos.x;
			tvec.y = self->target->so.pos.y - self->pos.y;
			space_obj_calc_dir(self);
			if (self->dir.y * tvec.x + self->type->width < self->dir.x * tvec.y)
				space_obj_rright(self);
			else if (self->dir.y * tvec.x - self->type->width > self->dir.x * tvec.y)
				space_obj_rleft(self);
			else {
				space_obj_thrust(self);
				if (so_projectile(self) != NULL)
					ret = space_obj_shoot(self);
			}
		}
	}
	space_obj_collisions(self, others);
	return ret;
}

static void sonode_unlink(struct space_obj_node *self)
{
	if (self->so.target != NULL)
		sonode_drop(self->so.target);
	if (self->rc == 0)
		free(self);
	else {
		self->so.target = NULL;
		self->next = NODE_UNLINKED;
	}
}

static void space_obj_update(struct space_obj *self, float width, float height)
{
	--self->lifetime;
	space_obj_move(self, width, height);
	if (so_projectile(self) != NULL)
		space_obj_do_reload(self);
}

int simulate_solist(struct space_obj_node *list, char last_key, struct canvas *c)
{
	struct space_obj_node *insert;
	space_obj_undraw_player(&list->so, c);
	if (space_obj_death(&list->so))
		return 0;
	space_obj_update(&list->so, c->width, c->height * 2);
	insert = space_obj_react_player(&list->so, list, last_key);
	if (insert) {
		insert->next = list->next;
		list->next = insert;
	}
	struct space_obj_node *node, *last_node;
	for (node = list->next, last_node = list; node != NULL; last_node = node, node = node->next) {
		space_obj_undraw(&node->so, c);
		if (space_obj_death(&node->so)) {
			last_node->next = node->next;
			sonode_unlink(node);
			node = last_node;
		} else {
			space_obj_update(&node->so, c->width, c->height * 2);
			insert = space_obj_react(&node->so, list);
			if (insert) {
				insert->next = node->next;
				node->next = insert;
			}
			space_obj_draw(&node->so, c);
		}
	}
	space_obj_draw_player(&list->so, c);
	return 1;
}

void init_solist(struct space_obj_node *list)
{
	list->next = NULL;
	list->rc = 0;
}

struct space_obj *sonode_inner(struct space_obj_node *self) { return &self->so; }

void push_to_solist(struct space_obj_node *list, struct space_obj_node *p)
{
	p->next = list->next;
	list->next = p;
}

int space_obj_print_stats(const struct space_obj *self, FILE *f)
{
	FORWARD(fprintf,(f, "HEALTH [%*c%*c\nRELOAD [%*c%*c\n",
			self->health + 1, '#', self->type->health - self->health + 1, ']',
			self->reload + 1, '#', self->type->reload_burst - self->reload + 1, ']'));
}

int space_obj_unprint_stats(const struct space_obj *self, FILE *f)
{
	FORWARD(fprintf,(f, "\x1B[2A"));
}

void drop_solist(struct space_obj_node *list)
{
	struct space_obj_node *next;
	for (list = list->next; list != NULL; list = next) {
		next = list->next;
		free(list);
	}
}
