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


#include "../game.h"

#include "noclip_move_component.h"


#define NOCLIP_SPEED 4


class MainState;

NoclipMoveComponent::NoclipMoveComponent(MainState* state, GameObject* obj)
    : LogicComponent(obj),
      _state(state) {
}

void NoclipMoveComponent::update() {
	Vec2& pos = _obj->geom().pos;
	if(left ->isPressed())  pos.x() -= NOCLIP_SPEED;
	if(right->isPressed())  pos.x() += NOCLIP_SPEED;
	if(up   ->isPressed())  pos.y() -= NOCLIP_SPEED;
	if(down ->isPressed())  pos.y() += NOCLIP_SPEED;
}
