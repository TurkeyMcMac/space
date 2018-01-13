#include "canvas.h"

#include "error.h"

#include <stdio.h>
#include <stdlib.h>

#define PRINT_STYLE(inner) "\x1B[" inner "m"

PIXEL pixel(char ch, char color)
{
	PIXEL p;
	p.ch = ch;
	p.color = color;
	p.inverted = 0;
	return p;
}

int print_pixel(PIXEL p, FILE *f)
{
	if (p.color == WHITE) {
		if (p.inverted)
			FORWARD (fprintf,(f, PRINT_STYLE("7") "%c" PRINT_STYLE("0"), p.ch));
		else
			FORWARD (fprintf,(f, "%c", p.ch));
	} else {
		if (p.inverted)
			FORWARD (fprintf,(f, PRINT_STYLE("7;3%c") "%c" PRINT_STYLE("0"), p.color, p.ch));
		else
			FORWARD (fprintf,(f, PRINT_STYLE("3%c") "%c" PRINT_STYLE("0"), p.color, p.ch));
	}
}

void canvas_init(struct canvas *c, size_t width, size_t height, PIXEL filler)
{
	c->width = width;
	c->height = height;
	c->pixels = malloc(width * height * sizeof(PIXEL));
	size_t i;
	for (i = 0; i < width * height; ++i)
		c->pixels[i] = filler;
}

PIXEL *canvas_get_unck(struct canvas *self, size_t x, size_t y)
{
	return &self->pixels[y * self->width + x];
}

PIXEL *canvas_get(struct canvas *self, size_t x, size_t y)
{
	if (x < self->width && y < self->height)
		return canvas_get_unck(self, x, y);
	else
		return NULL;
}

int canvas_print(const struct canvas *self, FILE *f)
{
	int last_write, total_write = 0;
	size_t x, y;
	for (y = 0; y < self->height; ++y) {
		for (x = 0; x < self->width; ++x)
			if CATCH_TO (last_write, print_pixel,(*canvas_get_unck((struct canvas *)self, x, y), f))
				return FAILURE;
			else
				total_write += last_write;
		if CATCH_TO (last_write, fprintf,(f, "\n"))
			return FAILURE;
		else
			total_write += last_write;
	}
	return total_write;
}

int canvas_unprint(const struct canvas *self, FILE *f)
{
	int total_write;
	if CATCH_TO (total_write, fprintf,(f, "\x1B[%luA", self->height))
		return FAILURE;
	else
		return total_write;
}
