#include "platform.h"
#include "splash.h"

#ifdef PLATFORM_NSWITCH

#include <stdlib.h>
#include <PR/ultratypes.h>
#include <SDL.h>
#include <zlib.h>
#include "splashdata.h"

#define SPLASH_DURATION_MS 2500

void splashShow(void)
{
	// SDL_Init is ref-counted; this is safe even though gfx_sdl2.cpp will
	// also call it later on during the normal videoInit() sequence.
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		return;
	}

	SDL_Window *win = SDL_CreateWindow("Perfect Dark", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SPLASH_WIDTH, SPLASH_HEIGHT, SDL_WINDOW_FULLSCREEN);
	if (!win) {
		return;
	}

	SDL_Surface *screenSurf = SDL_GetWindowSurface(win);
	if (!screenSurf) {
		SDL_DestroyWindow(win);
		return;
	}

	unsigned char *pixels = malloc(SPLASH_RAW_SIZE);
	if (!pixels) {
		SDL_DestroyWindow(win);
		return;
	}

	uLongf destLen = SPLASH_RAW_SIZE;
	const s32 ok = (uncompress(pixels, &destLen, splashDataCompressed, SPLASH_COMPRESSED_SIZE) == Z_OK);

	if (ok) {
		// raw 24bpp RGB, tightly packed, one row after another
		SDL_Surface *splashSurf = SDL_CreateRGBSurfaceFrom(
			pixels, SPLASH_WIDTH, SPLASH_HEIGHT, 24, SPLASH_WIDTH * 3,
			0x0000FF, 0x00FF00, 0xFF0000, 0);

		if (splashSurf) {
			SDL_BlitSurface(splashSurf, NULL, screenSurf, NULL);
			SDL_UpdateWindowSurface(win);
			SDL_FreeSurface(splashSurf);
		}
	}

	free(pixels);

	// wait for the duration, but bail out early on any button press, and keep
	// pumping events so the console doesn't consider the applet unresponsive
	const Uint32 startTicks = SDL_GetTicks();
	s32 skip = 0;
	while (!skip && (SDL_GetTicks() - startTicks) < SPLASH_DURATION_MS) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_KEYDOWN || ev.type == SDL_CONTROLLERBUTTONDOWN || ev.type == SDL_JOYBUTTONDOWN) {
				skip = 1;
			}
		}
		SDL_Delay(16);
	}

	SDL_DestroyWindow(win);
}

#else

void splashShow(void)
{
	// no-op on non-Switch platforms
}

#endif
