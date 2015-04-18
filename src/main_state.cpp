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


#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>

#include <SDL2/SDL_image.h>

#include "game.h"

#include "main_state.h"


#define MAIN_STATE_UPDATE_TIME  (1./60.)


MainState::MainState(Game* game)
    : GameState(game, durationFromSeconds(MAIN_STATE_UPDATE_TIME)),
      _pos(200, 0),
      _prevPos(200, 0),
      _sprite(nullptr) {
}


void MainState::update() {
	double alpha = 0.1;
	Eigen::Matrix2f rot;
	rot << cos(alpha), sin(alpha), -sin(alpha), cos(alpha);

	_prevPos = _pos;
	_pos = rot * _pos;
	_game->log("Update: ", _pos.transpose());
}

void MainState::frame(double interp) {
	_game->log("Frame: ", interp);

	Eigen::Vector2f pos = (1-interp) * _prevPos + interp * _pos;
	pos += Eigen::Vector2f(400, 300);

	SDL_Rect rect;
	rect.x = pos.x();
	rect.y = pos.y();
	rect.w = 32;
	rect.h = 32;

//	SDL_RenderClear(_game->renderer());
	SDL_RenderCopy(_game->renderer(), _sprite, nullptr, &rect);
	SDL_RenderPresent(_game->renderer());
}


void MainState::initialize() {
	_game->log("Initialize MainState...");

	SDL_Surface* surf = IMG_Load("./assets/test/character.png");
	if(!surf) _game->imgCrash("Failed to load image");
	_sprite = SDL_CreateTextureFromSurface(_game->renderer(), surf);
	if(!surf) _game->sdlCrash("Failed to create texture");
	SDL_FreeSurface(surf);
}


void MainState::shutdown() {
	_game->log("Shutdown MainState...");

	SDL_DestroyTexture(_sprite);
}


void MainState::start() {
	_game->log("Start MainState...");
}


void MainState::stop() {
	_game->log("Stop MainState...");
}
