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

#include "move_component.h"

#include "player_controler_component.h"


#define PLAYER_ANIM_FRAMES 20


class MainState;

PlayerControlerComponent::PlayerControlerComponent(
        MainState* state, GameObject* obj)
    : LogicComponent(obj),
      direction(P_FRONT),
      _state(state),
      _animCounter(0) {
}

void PlayerControlerComponent::update() {
	MoveComponent* ppm = static_cast<MoveComponent*>(
					_obj->getComponent(MOVE_COMPONENT_ID));
	assert(ppm);

	bool goLeft  = _state->input().isPressed(left);
	bool goRight = _state->input().isPressed(right);
	if(goLeft) {
		direction = P_LEFT;
		ppm->walk(LEFT);
	}
	if(goRight) {
		direction = P_RIGHT;
		ppm->walk(RIGHT);
	}
	
	if(_state->input().isPressed(jump)) ppm->jump();
	if(_state->input().isPressed(up))   ppm->climb(UP);
	if(_state->input().isPressed(down)) ppm->climb(DOWN);

	MoveComponent* pmc = static_cast<MoveComponent*>(_obj->getComponent(MOVE_COMPONENT_ID));
	if (pmc->_ladder)
		direction = P_BACK;

	if(goLeft || goRight) {
		++_animCounter;
	}
	else {
		_animCounter = 0;
	}
	switch(direction) {
	case P_FRONT:
		_obj->sprite->setTileIndex(0);
		break;
	case P_BACK:
		_obj->sprite->setTileIndex(1);
		break;
	case P_LEFT:
		_obj->sprite->setTileIndex(goLeft?
		            2 + (_animCounter / PLAYER_ANIM_FRAMES) % 2:
		            5);
		break;
	case P_RIGHT:
		_obj->sprite->setTileIndex(goRight?
		            6 + (_animCounter / PLAYER_ANIM_FRAMES) % 2:
		            9);
		break;
	}
}
