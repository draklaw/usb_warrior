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
	typedef void (*Command)(MainState*, unsigned, const char**);

public:
	MainState(Game* game);

	void update();
	void frame(double interp);

	inline GameObject* player()   { return _player; }
	inline Input       useInput() { return _use; }

	GameObject* getObject(const std::string& name);

	void loadLevel(const char* filename);
	void resetLevel();

	GameObject* createSpriteObject(const EntityData& data, const TileMap& tileMap);
	GameObject* createPlayer      (const EntityData& data);
	GameObject* createTrigger     (const EntityData& data);
	GameObject* createBotStatic   (const EntityData& data);
	GameObject* createWall        (const EntityData& data);

	void addCommand(const char* action, Command cmd);
	void exec(const char* cmd);

	inline InputManager& input() { return _input; }

protected:
	typedef std::unordered_map<std::string, GameObject*> ObjectMap;
	typedef std::unordered_map<std::string, Command>     CommandMap;

protected:
	void initialize();
	void shutdown();

	void start();
	void stop();

protected:
	Scene         _scene;

	Loader        _loader;

	CommandMap    _commandMap;
	std::string   _nextLevel;

public:
	bool          hasDeactivateKey;
//	bool          hasDeactivateKey;
//	bool          hasDeactivateKey;

protected:
	// Inputs
	InputManager  _input;

	Input         _left;
	Input         _right;
	Input         _jump;
	Input         _down;
	Input         _use;

	Input         _debug0;
	Input         _debug1;

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
