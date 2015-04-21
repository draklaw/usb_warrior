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

#ifndef _TEST_STATE_H_
#define _TEST_STATE_H_


#include <Eigen/Geometry>

#include <SDL2/SDL_render.h>

#include "input.h"
#include "image_manager.h"
#include "sound_player.h"
#include "font_manager.h"
#include "loader.h"
#include "game_state.h"
#include "scene.h"


class TestState : public GameState {
public:
	TestState(Game* game);

	void update();
	void frame(double interp);

protected:
	void initialize();
	void shutdown();

	void start();
	void stop();

protected:
	Scene         _scene;

	Loader        _loader;

	InputManager  _input;
	Input*        _left;
	Input*        _right;
	Input*        _up;
	Input*        _down;
	Input*        _use;

	TileMap       _tilemap;

	GameObject*   _obj;
	const Sound*  _msound;
	const Sound*  _jsound;
	const Music*  _music;
	int           _mchannel;
	
	Font          _font;
};


#endif
