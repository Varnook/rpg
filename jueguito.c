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

void fillBg(int wWidth, int wHeight, SDL_Texture* texture, SDL_Renderer* renderer) {
	SDL_Rect dstrect = { .x = 0, .y = 0, .w = 32, .h = 32};

	for (int i = 0; i < wWidth; i += 32) {
		for (int j = 0; j < wHeight; j += 32) {
			dstrect.x = i;
			dstrect.y = j;
			SDL_RenderCopy(renderer, texture, NULL, &dstrect);
		}
	}
}

int main (int argc, char* argv[]) {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event event;
	SDL_CreateWindowAndRenderer(10, 10, SDL_WINDOW_RESIZABLE, &window, &renderer);
	
	int wWidth, wHeight;
	int animationPos = 0;
	float animationMov = 0.25;

	square.rect.x = 0;
	square.rect.y = 0;
	square.rect.w = 64;
	square.rect.h = 64;

	SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "grassTiles.png");
	square.texture = IMG_LoadTexture(renderer, "ojo.png");

	SDL_Rect collisionBox = { .x = 0x200, .y = 0x200, .w = 64, .h = 64 };
	SDL_RendererFlip eyeFlip;

	while (handleEvent(event)) {
        SDL_PollEvent(&event);

		fillBg(wWidth, wHeight, bgTexture, renderer);

		SDL_GetWindowSize(window, &wWidth, &wHeight);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);
		SDL_RenderDrawRect(renderer, &collisionBox);

		SDL_Rect possibleMovement =  square.rect;
		
		if (square.up && square.rect.y - 4 >= 0) {
			possibleMovement.y -= 4;
			if (!SDL_HasIntersection(&possibleMovement, &collisionBox))
				square.rect.y -= 4;
		}

		if (square.down && square.rect.y + 4 <= wHeight - square.rect.h) {
			possibleMovement.y += 4;
			if (!SDL_HasIntersection(&possibleMovement, &collisionBox))
				square.rect.y += 4;
		}

		if (square.left && square.rect.x - 4 >= 0) {
			possibleMovement.x -= 4;
			if (!SDL_HasIntersection(&possibleMovement, &collisionBox))
				square.rect.x -= 4;
		}

		if (square.right && square.rect.x + 4 <= wWidth - square.rect.w) {
			possibleMovement.x += 4;
			if (!SDL_HasIntersection(&possibleMovement, &collisionBox))
				square.rect.x += 4;
		}


		if (square.right)
			eyeFlip = 0;
		if (square.left)
			eyeFlip = SDL_FLIP_HORIZONTAL;

		if (square.left | square.right | animationPos) {
			if (!(square.left | square.right) || animationPos <= -16) {
				animationMov = 1;
			} else if (animationPos >= 0) {
				animationMov = -1;
			}
			animationPos += animationMov;
		}

		SDL_Rect srcrect = { .x = 0, .y = 0, .w = 16, .h = 16};
		SDL_Rect dstrect = { .x = square.rect.x, .y = square.rect.y + animationPos, .w = square.rect.w, .h = square.rect.h };
		SDL_RenderCopyEx(renderer, square.texture, &srcrect, &dstrect, 0, NULL, eyeFlip);

        SDL_RenderPresent(renderer);
		SDL_Delay(16);

    }

	SDL_DestroyTexture(bgTexture);
	SDL_DestroyTexture(square.texture);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
