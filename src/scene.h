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

#ifndef _SCENE_H_
#define _SCENE_H_


#include <vector>

#include "game_object.h"
#include "sprite_component.h"


class Game;


class Scene {
public:
	Scene(Game* game);

	GameObject* addObject(const char* name = nullptr);

	void addSpriteComponent(GameObject* obj, const TileMap& tilemap,
	                        unsigned index = 0);

	void beginUpdate();
	void render(double interp);

private:
	typedef std::vector<GameObject>      ObjectVector;
	typedef std::vector<SpriteComponent> SpriteVector;

private:
	Game*         _game;

	std::size_t   _objectCounter;
	ObjectVector  _objects;

	SpriteVector  _sprites;
};


#endif
