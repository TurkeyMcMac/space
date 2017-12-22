#include "canvas.h"

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

void invert_pixel(PIXEL *p)
{
	p->inverted = ~p->inverted;
}

int print_pixel(PIXEL p, FILE *f)
{
	if (p.color == WHITE) {
		if (p.inverted)
			return fprintf(f, PRINT_STYLE("7") "%c" PRINT_STYLE("0"), p.ch);
		else
			return fprintf(f, "%c", p.ch);
	} else {
		if (p.inverted)
			return fprintf(f, PRINT_STYLE("7;3%c") "%c" PRINT_STYLE("0"), p.color, p.ch);
		else
			return fprintf(f, PRINT_STYLE("3%c") "%c" PRINT_STYLE("0"), p.color, p.ch);
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
	int write, total_write;
	size_t x, y;
	for (y = 0; y < self->height; ++y) {
		for (x = 0; x < self->width; ++x)
			if ((write = print_pixel(*canvas_get_unck((struct canvas *)self, x, y), f)) < 0)
				return write;
			else
				total_write += write;
		if ((write = fprintf(f, "\n")) < 0)
			return write;
		else
			total_write += write;
	}
	return total_write;
}

int canvas_unprint(const struct canvas *self, FILE *f)
{
	return fprintf(f, "\x1B[%luA", self->height);
}
