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


#include "sprite_component.h"

#include "game_object.h"


GameObject::GameObject(Scene* scene, const char* name)
    : sprite(0),
      sound(0),
      _scene(scene),
      _flags(0),
      _name(name) {
}


void GameObject::computeBoxFromSprite(const Vec2& anchor, float scale) {
	assert(sprite);
	Vec2 size = sprite->tilemap().tileSize().template cast<float>();
	Vec2 offset = scale * -(size.array() * anchor.array()).matrix();
	_geom[0].box = Boxf(offset, scale*size + offset);
}


void GameObject::_nextUpdate() {
	_geom[1] = _geom[0];
}
