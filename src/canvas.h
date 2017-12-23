#ifndef _CANVAS_H

#define _CANVAS_H

#include <stdio.h>

typedef enum {
	BLACK	= '0',
	RED	= '1',
	GREEN	= '2',
	YELLOW	= '3',
	BLUE	= '4',
	MAGENTA	= '5',
	CYAN 	= '6',
	WHITE 	= '7',
} COLOR_CODE;

typedef struct {
	char ch;
	char color : 7;
	char inverted : 1;
} PIXEL;

PIXEL pixel(char ch, char color);

int print_pixel(PIXEL p, FILE *f);

struct canvas {
	size_t width, height;
	PIXEL *pixels;
};

void canvas_init(struct canvas *c, size_t width, size_t height, PIXEL filler);

PIXEL *canvas_get_unck(struct canvas *self, size_t x, size_t y);

PIXEL *canvas_get(struct canvas *self, size_t x, size_t y);

int canvas_print(const struct canvas *self, FILE *f);

int canvas_unprint(const struct canvas *self, FILE *f);

#endif
