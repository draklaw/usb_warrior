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

#include "move_component.h"

#include "../main_state.h"

#define ACC_GRAVITY 0.0981f // px / fr^2

// A jump lets you move upward for half a second.
#define ACC_JUMP 0.5*ACC_GRAVITY / UPDATE_TIME

#define ACC_WALK 0.1f
#define ACC_DASH 0.3f

MoveComponent::MoveComponent(GameObject* obj)
    : LogicComponent(obj) {
	_puppet =  &_obj->geom(CURR_UP);
	_mSpeed = Vec2(0,0);
	_ground = true;
}

void MoveComponent::update() {
	_puppet->pos += _mSpeed;
	
	if (_puppet->pos.y() > 560.0)
	{
		_puppet->pos.y() = 560.0;
		_ground = true;
		_mSpeed.y() = 0;
	}
	
	_mSpeed += Vec2(0,ACC_GRAVITY);
}

void MoveComponent::jump() {
	if (_ground)
		_mSpeed += Vec2(0,-ACC_JUMP);
	_ground = false;
}

void MoveComponent::walk(Direction d) {
	switch (d) {
		case LEFT:
			_mSpeed += Vec2(-ACC_WALK,0);
			break;
		case RIGHT:
			_mSpeed += Vec2(ACC_WALK,0);
			break;
		default:;
	}
}

void MoveComponent::sprint(Direction d) {
	//TODO: Implement.
}
