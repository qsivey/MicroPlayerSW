#ifndef __PNGLE_H__
#define __PNGLE_H__

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

// Main Pngle object
typedef struct _pngle_t pngle_t;

extern const size_t PNGLE_T_SIZE;  // size of pngle_t structure

// Callback signatures
typedef void (*pngle_init_callback_t)(pngle_t *pngle, uint32_t w, uint32_t h);
typedef void (*pngle_draw_callback_t)(pngle_t *pngle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const uint8_t rgba[4]);
typedef void (*pngle_done_callback_t)(pngle_t *pngle);

// ----------------
// Basic interfaces
// ----------------
pngle_t *pngle_new();
void pngle_destroy(pngle_t *pngle);
void pngle_reset(pngle_t *pngle); // clear its internal state (not applied to pngle_set_* functions)
const char *pngle_error(pngle_t *pngle);
int pngle_feed(pngle_t *pngle, const void *buf, size_t len); // returns -1: On error, 0: Need more data, n: n bytes eaten

uint32_t pngle_get_width(pngle_t *pngle);
uint32_t pngle_get_height(pngle_t *pngle);
const uint8_t *pngle_get_background_color(pngle_t *pngle);

void pngle_set_init_callback(pngle_t *png, pngle_init_callback_t callback);
void pngle_set_draw_callback(pngle_t *png, pngle_draw_callback_t callback);
void pngle_set_done_callback(pngle_t *png, pngle_done_callback_t callback);

void pngle_set_display_gamma(pngle_t *pngle, double display_gamma); // enables gamma correction by specifying display gamma, typically 2.2. No effect when gAMA chunk is missing

void pngle_set_user_data(pngle_t *pngle, void *user_data);
void *pngle_get_user_data(pngle_t *pngle);

// ----------------
// Debug interfaces
// ----------------

typedef struct _pngle_ihdr_t {
	uint32_t width;
	uint32_t height;
	uint8_t depth;
	uint8_t color_type;
	uint8_t compression;
	uint8_t filter;
	uint8_t interlace;
} pngle_ihdr_t;

// Get IHDR information
pngle_ihdr_t *pngle_get_ihdr(pngle_t *pngle);

#ifdef __cplusplus
}
#endif

#endif /* __PNGLE_H__ */
