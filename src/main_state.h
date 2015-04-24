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


#include <memory>
#include <unordered_map>

#include <Eigen/Geometry>

#include <SDL2/SDL_render.h>

//#include "input.h"
//#include "image_manager.h"
//#include "sound_player.h"
//#include "font_manager.h"
//#include "loader.h"
#include "game_state.h"
//#include "level.h"
//#include "scene.h"


class GameObject;
class Loader;
class Scene;
class InputManager;
class Input;


class MainState : public GameState {
public:
	MainState(Game* game);
	~MainState();

	void update();
	void frame(double interp);

	inline GameObject* player()    { return _player; }
	inline Input*      useInput()  { return _use; }
	inline Loader*     loader()    { return _loader.get(); }

	inline InputManager& input() { return *_input; }

	void loadLevelNow(const char*);

protected:
	void initialize();
	void shutdown();

	void start();
	void stop();

protected:
	std::unique_ptr<Scene>   _scene;

	std::unique_ptr<Loader>  _loader;

	std::string   _level;
	bool          _reload;

public:
	bool          hasDeactivateKey;
	bool          hasComputerKey;
	bool          hasFightClubKey;
	bool          hasMysteryKey;

protected:
	// Inputs
	std::unique_ptr<InputManager>  _input;

public:
	Input*        _left;
	Input*        _right;
	Input*        _jump;
	Input*        _up;
	Input*        _down;
	Input*        _use;

private:
	Input*        _debug0;
	Input*        _debug1;

	// Objects
	GameObject*   _player;
};


#endif
