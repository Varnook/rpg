#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>
#define PRESSED  1
#define RELEASED 0

struct Sprite {
	SDL_Rect rect;
	int up, down, left, right;
	SDL_Texture* texture;
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
	
	int wWidth, wHeight;

	square.rect.x = 500;
	square.rect.y = 500;
	square.rect.w = 100;
	square.rect.h = 100;

	square.texture = IMG_LoadTexture(renderer, "drawing.svg");

	SDL_Rect collisionBox = { .x = 600, .y = 600, .w = 40, .h = 40 };

	int srcY[6] = {0, 188, 376, 564, 376, 188};
	int gifFrame = 0;
	int frameCounter = 0;

	while (handleEvent(event)) {
        SDL_PollEvent(&event);

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);

		SDL_GetWindowSize(window, &wWidth, &wHeight);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);
		SDL_RenderDrawRect(renderer, &collisionBox);

		SDL_Rect possibleMovement =  square.rect;
		
		if (square.up && square.rect.y - 4 >= 0) {
		//	possibleMovement.y -= 4;
		//	if (!SDL_HasIntersection(&possibleMovement, &collisionBox))
				square.rect.y -= 4;
		}

		if (square.down && square.rect.y + 4 <= wHeight - square.rect.h) {
		//	possibleMovement.y += 4;
		//	if (!SDL_HasIntersection(&possibleMovement, &collisionBox))
				square.rect.y += 4;
		}

		if (square.left && square.rect.x - 4 >= 0) {
		//	possibleMovement.x -= 4;
		//	if (!SDL_HasIntersection(&possibleMovement, &collisionBox))
				square.rect.x -= 4;
		}

		if (square.right && square.rect.x + 4 <= wWidth - square.rect.w) {
		//	possibleMovement.x += 4;
		//	if (!SDL_HasIntersection(&possibleMovement, &collisionBox))
				square.rect.x += 4;
		}

		SDL_Rect srcrect = { 0, srcY[gifFrame], 174, 159 };
		SDL_RenderCopy(renderer, square.texture, &srcrect, &square.rect);

        SDL_RenderPresent(renderer);
		SDL_Delay(16);

		frameCounter = frameCounter == 59 ? 0 : frameCounter + 1;
		gifFrame = frameCounter / 5;
    }

	SDL_DestroyTexture(square.texture);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
