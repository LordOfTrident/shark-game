#include <stdlib.h>  /* EXIT_SUCCESS, EXIT_FAILURE */
#include <stdbool.h> /* bool, true, false */
#include <stdio.h>   /* printf, fprintf, stderr */
#include <string.h>  /* strcmp, strcpy, strcat */

#include <SDL2/SDL.h>

#define WIN_W 625
#define WIN_H 604

#define TITLE            "SHARK GAME"
#define DEFAULT_RES_PATH "res"

typedef struct {
	SDL_Window   *win;
	SDL_Renderer *ren;
	SDL_Event     evt;

	SDL_Texture *texture;

	bool running;

	const char *res_path;
} state_t;

int init(state_t *state) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_Log("Error while initializing: %s", SDL_GetError());
		return -1;
	}

	state->win = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	                              WIN_W, WIN_H, SDL_WINDOW_RESIZABLE);
	if (state->win == NULL) {
		SDL_Log("Error while creating a window: %s", SDL_GetError());
		return -1;
	}

	state->ren = SDL_CreateRenderer(state->win, -1, SDL_RENDERER_ACCELERATED);
	if (state->ren == NULL) {
		SDL_Log("Error while creating a renderer: %s", SDL_GetError());
		return -1;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	if (SDL_RenderSetLogicalSize(state->ren, WIN_W, WIN_H) < 0) {
		SDL_Log("Error while setting logical render size: %s", SDL_GetError());
		return -1;
	}

	char path[1024];
	strcpy(path, state->res_path);
	strcat(path, "/img.bmp");

	SDL_Surface *s = SDL_LoadBMP(path);
	if (s == NULL) {
		SDL_Log("Error while loading image: %s", SDL_GetError());
		return -1;
	}

	state->texture = SDL_CreateTextureFromSurface(state->ren, s);
	if (state->texture == NULL) {
		SDL_Log("Error while creating texture: %s", SDL_GetError());
		return -1;
	}

	SDL_FreeSurface(s);
	state->running = true;
	return 0;
}

void cleanup(state_t *state) {
	SDL_DestroyTexture(state->texture);
	SDL_DestroyRenderer(state->ren);
	SDL_DestroyWindow(state->win);
	SDL_Quit();
}

void render(state_t *state) {
	SDL_RenderClear(state->ren);
	SDL_RenderCopy(state->ren, state->texture, NULL, NULL);
	SDL_RenderPresent(state->ren);
}

void handle_events(state_t *state) {
	while (SDL_PollEvent(&state->evt)) {
		switch (state->evt.type) {
		case SDL_QUIT: state->running = false; break;
		case SDL_KEYDOWN:
			if (state->evt.key.keysym.sym == SDLK_RETURN)
				state->running = false;
			break;

		default: break;
		}
	}
}

int main(int argc, char **argv) {
	state_t state = {.res_path = DEFAULT_RES_PATH};

	if (argc == 2) {
		if (strcmp(argv[1], "-h") == 0) {
			printf("Usage: [RES_PATH | -h]\n  RES_PATH: Resources folder path\n");
			return EXIT_SUCCESS;
		} else
			state.res_path = (const char*)argv[1];
	} else if (argc > 2) {
		fprintf(stderr, "Error: Unexpected argument '%s'\n", argv[2]);
		return EXIT_FAILURE;
	}

	if (init(&state) != 0)
		return EXIT_FAILURE;

	while (state.running) {
		render(&state);
		handle_events(&state);

		SDL_Delay(1000 / 60);
	}

	cleanup(&state);
	return EXIT_SUCCESS;
}
