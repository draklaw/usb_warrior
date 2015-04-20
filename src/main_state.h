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

#ifndef _MAIN_STATE_H_
#define _MAIN_STATE_H_


#include <unordered_map>

#include <Eigen/Geometry>

#include <SDL2/SDL_render.h>

#include "input.h"
#include "image_manager.h"
#include "sound_player.h"
#include "font_manager.h"
#include "loader.h"
#include "game_state.h"
#include "scene.h"


class MainState : public GameState {
public:
	MainState(Game* game);

	void update();
	void frame(double interp);

	void loadLevel(const char* filename);
	void resetLevel();

	GameObject* createSpriteObject(const EntityData& data, const TileMap& tileMap);
	GameObject* createPlayer      (const EntityData& data);
	GameObject* createExit        (const EntityData& data);
	GameObject* createTP          (const EntityData& data);

	GameObject* createBotStatic(const EntityData& data);

	inline InputManager& input() { return _input; }

protected:
	typedef std::unordered_map<std::string, GameObject*> ObjectMap;

protected:
	void initialize();
	void shutdown();

	void start();
	void stop();

protected:
	Scene         _scene;

	Loader        _loader;

	// Inputs
	InputManager  _input;

	Input         _left;
	Input         _right;
	Input         _jump;
	Input         _use;
	Input         _debug;

	// Objects
	ObjectMap     _objects;
	GameObject*   _player;
	
	// TileMaps
	TileMap       _playerTileMap;
	TileMap       _exitTileMap;

	// Other
	Font          _font;
};


#endif
