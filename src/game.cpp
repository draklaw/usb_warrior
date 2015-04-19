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
#include <cassert>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_render.h>

#include <SDL2/SDL_image.h>

#include "game_state.h"
#include "soundplayer.h"

#include "game.h"


#define SOUNDPLAYER_MAX_CHANNELS 32


Game::Game(int /*argc*/, char** /*argv*/)
    : _window(nullptr),
      _renderer(nullptr),
      _imageManager(this),
      _state(nullptr),
      _nextState(nullptr),
	  _player(nullptr) {
}


Game::~Game() {
	shutdown();
}


void Game::initialize() {
	unsigned initFlags = SDL_INIT_VIDEO
	                   | SDL_INIT_AUDIO
	                   | SDL_INIT_EVENTS;

	log("Initialize SDL...");
	if(SDL_Init(initFlags)) {
		sdlCrash("Failed to initialize SDL");
	}

	log("Initialize SDL_image...");
	int initImgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
	if(IMG_Init(initImgFlags) != initImgFlags) {
		imgCrash("Failed to initialize SDL_image backend");
	}

	log("Initialize SDL_mixer...");
	_player = new SoundPlayer;

	unsigned windowFlags = 0
#ifdef NDEBUG
	        | SDL_WINDOW_FULLSCREEN_DESKTOP
#endif
//			| SDL_WINDOW_OPENGL
	        ;

	log("Create window...");
	_window = SDL_CreateWindow(
	            "USB Warrior",
	            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	            800, 600,
	            windowFlags);
	if(!_window) { sdlCrash("Failed to create window"); }

	log("Create renderer...");
	_renderer = SDL_CreateRenderer(_window, -1,
	        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if(!_renderer) { sdlCrash("Failed to create renderer"); }
}


void Game::shutdown() {
	if(_renderer) {
		log("Destroy renderer...");
		SDL_DestroyRenderer(_renderer);
		_renderer = nullptr;
	}
	if(_window) {
		log("Destroy window...");
		SDL_DestroyWindow(_window);
		_window = nullptr;
	}

	log("Quit SDL_mixer...");
	delete _player;

	log("Quit SDL_image...");
	IMG_Quit();

	log("Quit SDL...");
	SDL_Quit();
}


void Game::dispatchPendingEvents() {
	SDL_Event event;
	while(SDL_PollEvent(&event)) {
		switch(event.type) {
		case SDL_QUIT:
			quit();
			break;
		}
	}
}


void Game::changeState(GameState* nextState) {
	assert(nextState);
	if(!_state) {
		warning("Try to change to state \"", nextState->name(),
		        "\" after quit has been called");
		return;
	}
	if(_nextState) {
		warning("Changing to state \"", nextState->name(),
		        "\" while an other state is planned: \"", _nextState->name(), "\"");
	}
	_nextState = nextState;
}


int Game::run(GameState* state) {
	assert(state);

	_nextState = state;
	while(_nextState) {
		_state     = _nextState;
		_nextState = nullptr;
		log("Running state \"", _state->name(), "\"");
		_state->run();
	}

	return EXIT_SUCCESS;
}


void Game::quit() {
	if(_state) {
		_state->quit();
		_state = nullptr;
		_nextState = nullptr;
	}
}


int Game::getRefreshRate() const{
	return 60;
}


void Game::sdlCrash(const char* msg) {
	error(msg, ": ", SDL_GetError());
	std::exit(EXIT_FAILURE);
}


void Game::imgCrash(const char* msg) {
	error(msg, ": ", IMG_GetError());
	std::exit(EXIT_FAILURE);
}
