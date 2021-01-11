#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>
#define PRESSED  1
#define RELEASED 0

struct Sprite {
	int posX, posY,
		up, down, left, right;
} square;

void handleKey(int state, SDL_KeyboardEvent *keyEvent) {
	if (keyEvent->keysym.scancode == SDL_SCANCODE_W)
		square.up    = state ? 1 : 0;

	if (keyEvent->keysym.scancode == SDL_SCANCODE_S)
		square.down  = state ? 1 : 0;

	if (keyEvent->keysym.scancode == SDL_SCANCODE_A)
		square.left  = state ? 1 : 0;

	if (keyEvent->keysym.scancode == SDL_SCANCODE_D)
		square.right = state ? 1 : 0;
}

int handleEvent(SDL_Event event) {
	switch (event.type) {
		case SDL_QUIT:
			return 0;
		case SDL_KEYDOWN:
			handleKey(PRESSED, &event.key);
			return 1;
		case SDL_KEYUP:
			handleKey(RELEASED, &event.key);
			return 1;
		default:
			return 1;
	}
}

int main (int argc, char* argv[]) {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event event;
	SDL_CreateWindowAndRenderer(10, 10, SDL_WINDOW_RESIZABLE, &window, &renderer);
	
	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	int ancho = 10;

	square.posX = 500;
	square.posY = 500;
	SDL_Texture* texture = IMG_LoadTexture(renderer, "pic.png");

	while (handleEvent(event)) {
        SDL_PollEvent(&event);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);

		if (square.up)
			square.posY -= 4;

		if (square.down)
			square.posY += 4;

		if (square.left)
			square.posX -= 4;

		if (square.right)
			square.posX += 4;

		SDL_Rect dest = { .x = square.posX, .y = square.posY, .w = 30, .h = 30 };
		SDL_RenderCopy(renderer, texture, NULL, &dest);

        SDL_RenderPresent(renderer);
		SDL_Delay(16);
    }

	SDL_Quit();
	return 0;
}
