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

#include "utils.h"
#include "game.h"

#include "main_state.h"


#define MAIN_STATE_UPDATE_TIME  (1./60.)


MainState::MainState(Game* game)
    : GameState(game, durationFromSeconds(MAIN_STATE_UPDATE_TIME)),
      _scene(game),
      _obj(nullptr) {
}


void MainState::update() {
	_scene.beginUpdate();

	double alpha = 0.05;
	Eigen::Matrix2f rot;
	rot << cos(alpha), sin(alpha), -sin(alpha), cos(alpha);

	Vec2 center(400, 300);
	_obj->geom().pos = rot * (_obj->geom().pos - center) + center;
}

void MainState::frame(double interp) {
	_scene.render(interp);
}


void MainState::initialize() {
	_game->log("Initialize MainState...");

	_tilemap = _game->images()->loadTilemap("assets/test/tileset.png", 32, 32);

	_obj = _scene.addObject("Test");
	_scene.addSpriteComponent(_obj, _tilemap, 3);
	_obj->computeBoxFromSprite(Vec2(.5, .5), 2);
	_obj->geom().pos = Vec2(500, 300);
}


void MainState::shutdown() {
	_game->log("Shutdown MainState...");

	_game->images()->releaseTilemap(_tilemap);
}


void MainState::start() {
	_game->log("Start MainState...");
}


void MainState::stop() {
	_game->log("Stop MainState...");
}
