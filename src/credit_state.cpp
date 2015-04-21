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


#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>

#include "game.h"

#include "credit_state.h"


CreditState::CreditState(Game* game)
    : GameState(game, "Credits", durationFromSeconds(1)) {
}

void CreditState::update() {
	// DO NOT USE
}

void CreditState::frame(double /*interp*/) {
	if(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_ESCAPE]) {
		quit();
		_game->quit();
	}
	if(titleScreen && SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_SPACE]) {
		_game->changeState(&_game->mainState);
		quit();
	}

	Vec2i screenSize = _game->screenSize();

	SDL_Rect sRect;
	sRect.x = 0;
	sRect.y = 0;
	sRect.w = _splash->size.x();
	sRect.h = _splash->size.y();

	SDL_Rect dRect;
	dRect.x = 0;
	dRect.y = 0;
	dRect.w = screenSize.x();
	dRect.h = screenSize.y();

	SDL_RenderCopy(_game->renderer(), _splash->texture, &sRect, &dRect);
	SDL_RenderPresent(_game->renderer());
}

void CreditState::initialize() {
	_splash = _game->images()->loadImage(image);
	_music = _game->sounds()->loadMusic("assets/truc.wav");
}

void CreditState::shutdown() {
	_game->images()->releaseImage(_splash);
	_game->sounds()->releaseMusic(_music);
}

void CreditState::start() {
	_game->sounds()->playMusic(_music);
}

void CreditState::stop() {
	_game->sounds()->haltMusic();
}
