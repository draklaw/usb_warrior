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
      _input(game),
      _left(INVALID_INPUT),
      _right(INVALID_INPUT),
      _up(INVALID_INPUT),
      _down(INVALID_INPUT),
      _obj(nullptr) {
}


void MainState::update() {
	_scene.beginUpdate();

	_input.sync();

	double speed = 4;

	if(_input.isPressed(_left))  _obj->geom().pos.x() -= speed;
	if(_input.isPressed(_right)) _obj->geom().pos.x() += speed;
	if(_input.isPressed(_up))    _obj->geom().pos.y() -= speed;
	if(_input.isPressed(_down))  _obj->geom().pos.y() += speed;

}


void MainState::frame(double interp) {
	_scene.render(interp);
}


void MainState::initialize() {
	_game->log("Initialize MainState...");

	_left  = _input.addInput("left");
	_right = _input.addInput("right");
	_up    = _input.addInput("up");
	_down  = _input.addInput("down");

	_input.mapScanCode(_left,  SDL_SCANCODE_LEFT);
	_input.mapScanCode(_right, SDL_SCANCODE_RIGHT);
	_input.mapScanCode(_up,    SDL_SCANCODE_UP);
	_input.mapScanCode(_down,  SDL_SCANCODE_DOWN);

	_tilemap = _game->images()->loadTilemap("assets/test/tileset.png", 32, 32);

	_obj = _scene.addObject("Test");
	_scene.addSpriteComponent(_obj, _tilemap, 3);
	_obj->computeBoxFromSprite(Vec2(.5, .5), 2);
	_obj->geom().pos = Vec2(400, 300);
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
