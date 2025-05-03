#ifndef __FONT_H
#define __FONT_H

#include <stdint.h>

typedef struct {
	const uint8_t *data;
	uint16_t width;
	uint16_t height;
	uint8_t dataSize;
 } tImage;

typedef struct {
	long int code;
	const tImage *image;
} tChar;

typedef struct {
	int length;
	const tChar *chars;
} tFont;

extern tFont Font2;
//extern tFont Font3;
extern tFont Font4;
extern tFont Font20;
#endif
