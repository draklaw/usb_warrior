/*
 *  Copyright (C) 2015 the authors
 *
 *  This file is part of usb_warrior.
 *
 *  usb_warrior is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  usb_warrior is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with usb_warrior.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <cstdlib>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_render.h>

#include <SDL2/SDL_image.h>

#include "game.h"


Game::Game(int /*argc*/, char** /*argv*/)
    : _window(nullptr), _renderer(nullptr) {
}


Game::~Game() {
	quit();
}


void Game::init() {
	unsigned initFlags = SDL_INIT_VIDEO
	                   | SDL_INIT_AUDIO
	                   | SDL_INIT_EVENTS;

	if(SDL_Init(initFlags)) {
		sdlCrash("Failed to initialize SDL");
	}

	int initImgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
	if(IMG_Init(initImgFlags) != initImgFlags) {
		imgCrash("Failed to initialize .png and .jpg backend");
	}

	unsigned windowFlags = 0
#ifdef NDEBUG
	        | SDL_WINDOW_FULLSCREEN_DESKTOP
#endif
//			| SDL_WINDOW_OPENGL
	        ;
	_window = SDL_CreateWindow(
	            "USB Warrior",
	            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	            800, 600,
	            windowFlags);
	if(!_window) { sdlCrash("Failed to create window"); }

	_renderer = SDL_CreateRenderer(_window, -1,
	        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if(!_renderer) { sdlCrash("Failed to create renderer"); }
}


void Game::quit() {
	if(_renderer) {
		SDL_DestroyRenderer(_renderer);
		_renderer = nullptr;
	}
	if(_window) {
		SDL_DestroyWindow(_window);
		_window = nullptr;
	}

}


int Game::run() {
	SDL_Surface* surf = IMG_Load("./assets/splash.png");
	if(!surf) imgCrash("Failed to load image");
	SDL_Texture* testTex = SDL_CreateTextureFromSurface(_renderer, surf);
	if(!surf) sdlCrash("Failed to create texture");
	SDL_FreeSurface(surf);

	SDL_RenderCopy(_renderer, testTex, nullptr, nullptr);
	SDL_RenderPresent(_renderer);

	SDL_Event event;
	bool running = true;
	while(running) {
		SDL_WaitEvent(&event);

		std::cout << "Event: " << event.type << "\n";

		switch(event.type) {
			case SDL_QUIT: running = false; break;
		}
	}

	return EXIT_SUCCESS;
}


void Game::sdlCrash(const char* msg) {
	std::cerr << msg << ": " << SDL_GetError() << "\n";
	std::exit(EXIT_FAILURE);
}


void Game::imgCrash(const char* msg) {
	std::cerr << msg << ": " << IMG_GetError() << "\n";
	std::exit(EXIT_FAILURE);
}
