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

LogicComponent::LogicComponent(GameObject* obj)
	: _obj(obj) {
}

GameObject::GameObject(Scene* scene, const char* name)
    : sprite(0),
      sound(0),
      _scene(scene),
      _flags(OBJECT_ACTIVE),
      _name(name) {
}


void GameObject::computeBoxFromSprite(const Vec2& anchor, float scale) {
	assert(sprite);
	Vec2 size = sprite->tilemap().tileSize().template cast<float>();
	Vec2 offset = scale * -(size.array() * anchor.array()).matrix();
	_geom[0].box = Boxf(offset, scale*size + offset);
}


bool GameObject::hasComponent(unsigned id) const {
	return id < _logicMap.size() && _logicMap[id];
}


void GameObject::setActive(bool active) {
	if(active) _flags |=  OBJECT_ACTIVE;
	else       _flags &= ~OBJECT_ACTIVE;
}


void GameObject::_nextUpdate() {
	_geom[1] = _geom[0];
}


void GameObject::_registerLogic(unsigned id, LogicComponent* lcomp) {
	if (_logicMap.size() <= id) {
		_logicMap.resize(id, nullptr);
	}
	
	_logicMap[id] = lcomp;
}
