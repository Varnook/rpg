#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <stdio.h>

// Constantes 
#define PRESSED  1
#define RELEASED 0

#define LOAD 1
#define FREE 1

#define ARRIBA -1
#define ABAJO   1

struct Personaje {
	SDL_Rect hitbox;
	int up, down, left, right;
	unsigned int banderasAnimacion;
	// ←... ---- ---1 XXXX 1XX1 DXXX
	//              ┃      ┃  ┃
	//              ┃      ┃  ┗▶ Flota, direccion, píxeles
	//              ┃      ┗━━━▶ Orientación, 4 direcciones
	//              ┗━━━━━━━━━━▶ Animado, frames
	SDL_Rect textureRect;
	SDL_Texture* texture;
};

void textureBank(int load, SDL_Renderer* renderer, int length, char** files, SDL_Texture** textures) {
	if (load)
		for (int i = 0; i < length; i++)
			textures[i] = IMG_LoadTexture(renderer, files[i]);
}

void freeTextures(int free, int length, SDL_Texture** textures) {
	if (free)
		for (int i = 0; i < length; i++) 
			SDL_DestroyTexture(textures[i]);
}

void handleKey(int state, SDL_KeyboardEvent *keyEvent, struct Personaje* jugador) {
	if (keyEvent->keysym.scancode == SDL_SCANCODE_W)
		jugador->up    = state ? 1 : 0;

	if (keyEvent->keysym.scancode == SDL_SCANCODE_S)
		jugador->down  = state ? 1 : 0;

	if (keyEvent->keysym.scancode == SDL_SCANCODE_A)
		jugador->left  = state ? 1 : 0;

	if (keyEvent->keysym.scancode == SDL_SCANCODE_D)
		jugador->right = state ? 1 : 0;
}

int handleEvent(SDL_Event event, struct Personaje* jugador) {
	switch (event.type) {
		case SDL_QUIT:
			return 0;
		case SDL_KEYDOWN:
			handleKey(PRESSED, &event.key, jugador);
			return 1;
		case SDL_KEYUP:
			handleKey(RELEASED, &event.key, jugador);
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

void movPersonaje(int wWidth, int wHeight, struct Personaje* pj) {
	SDL_Rect possibleMovement =  pj->hitbox;
	
	if (pj->right)
		pj->banderasAnimacion &= 0xFFFFFFFF9F;

	if (pj->left)  {
		pj->banderasAnimacion &= 0xFFFFFFFF9F;
		pj->banderasAnimacion |= 0x40;
	}

	if (pj->up && pj->hitbox.y - 4 >= 0)
		pj->hitbox.y -= 4;

	if (pj->down && pj->hitbox.y + 4 <= wHeight - pj->hitbox.h)
		pj->hitbox.y += 4;

	if (pj->left && pj->hitbox.x - 4 >= 0)
		pj->hitbox.x -= 4;

	if (pj->right && pj->hitbox.x + 4 <= wWidth - pj->hitbox.w)
		pj->hitbox.x += 4;
}

void dibujarPersonaje(struct Personaje* pj, SDL_Renderer* renderer) {
	SDL_RendererFlip pjFlip;
	switch (pj->banderasAnimacion & 0x00000060) {
		case 0:
			pjFlip = 0;
			break;
		case 0x40:
			pjFlip = SDL_FLIP_HORIZONTAL;
			break;
		default:
			pjFlip = 0;
			break;
	}
	// ←... ---- ---1 XXXX 1XX1 DXXX D = 1, sube
	if (pj->left | pj->right | (pj->banderasAnimacion & 0x00000007)) {
		if (!(pj->left | pj->right) || (pj->banderasAnimacion & 0x00000007) == 0x7)
			pj->banderasAnimacion &= 0xFFFFFFF7;
		else if ((pj->banderasAnimacion & 0x00000007) == 0x0)
			pj->banderasAnimacion |= 0x00000008;

		if (pj->banderasAnimacion & 0x00000008)
			pj->banderasAnimacion++;
		else
			pj->banderasAnimacion--;
	}

	pj->textureRect.x = pj->hitbox.x - 8;
	pj->textureRect.y = pj->hitbox.y - 8 - (pj->banderasAnimacion & 0x7);
	SDL_RenderCopyEx(renderer, pj->texture, NULL, &pj->textureRect, 0, NULL, pjFlip);
}

int main (int argc, char *argv[]) {
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Event event = {};
	SDL_CreateWindowAndRenderer(10, 10, SDL_WINDOW_RESIZABLE, &window, &renderer);
	
	int wWidth, wHeight;

	char* files[2] = {"ojo.png", "grassTiles.png"};
	SDL_Texture* textures[2];
	textureBank(LOAD, renderer, 2, files, textures);

	struct Personaje jugador = {
		.hitbox = { .x = 8, .y = 8, .w = 48, .h = 48 },
		.up = 0, .down = 0, .left = 0, .right = 0,
		.banderasAnimacion = 0x00001090,
		.textureRect = { .x = 0, .y = 0, .w = 64, .h = 64 },
		.texture = textures[0],
	};

	SDL_Texture* bgTexture = textures[1];

	Uint32 timeStepMs = 1000 / 60;
	Uint32 nextGameStep = SDL_GetTicks();

	while (handleEvent(event, &jugador)) {

        SDL_PollEvent(&event);

		SDL_GetWindowSize(window, &wWidth, &wHeight);
		
		movPersonaje(wWidth, wHeight, &jugador);
		Uint32 now = SDL_GetTicks();

		if (nextGameStep <= now) {
			fillBg(wWidth, wHeight, bgTexture, renderer);
			dibujarPersonaje(&jugador, renderer);
			SDL_RenderPresent(renderer);
		}

		nextGameStep += timeStepMs;
		SDL_Delay(nextGameStep - now);
    }

	freeTextures(FREE, 2, textures);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
