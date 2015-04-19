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

#ifndef _SPRITE_COMPONENT_H_
#define _SPRITE_COMPONENT_H_


#include "image_manager.h"


class GameObject;


class SpriteComponent {
public:
	SpriteComponent(GameObject* object, const TileMap& tilemap = TileMap(),
	                unsigned tileIndex = 0);

	GameObject*    object()    const { return _object; }
	const TileMap& tilemap()   const { return _tilemap; }
	// Index can be invalid.
	unsigned       tileIndex() const { return _tileIndex; }

	void setTilemap(const TileMap& tilemap);
	void setTileIndex(unsigned index);

private:
	GameObject* _object;
	TileMap     _tilemap;
	unsigned    _tileIndex;
};


#endif
