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

#include "player_controler_component.h"

#include "bot_component.h"



class MainState;

BotComponent::BotComponent(MainState* state, GameObject* obj)
    : LogicComponent(obj),
      _state(state),
	  _channel(-1)  {
}

void BotComponent::update() {
	_obj->sprite->setTileIndex(direction);

	Vec2 lookDir = Vec2::UnitX();
	if(!direction) lookDir *= -1;

	Vec2 pos = _obj->geom().pos;
	Vec2 pPos = _state->player()->geom().pos;

	if(!seePlayer.empty()) {
		float cosAlpha = (pPos - pos).normalized().dot(lookDir);
		float alpha = std::acos(cosAlpha) / M_PI * 180;

		if(alpha < fov) {
			_state->exec(seePlayer.c_str());
			if(_channel == -1) {
				const Sound* alarmSnd = _state->getLoader().getSound("assets/alarm.wav");
				_channel = _state->game()->sounds()->playSound(alarmSnd, -1);
			}
		}
	}

	if(_state->input().justPressed(_state->useInput())) {
		auto pcc = static_cast<PlayerControlerComponent*>(
					_state->player()->getComponent(PLAYER_CONTROLLER_COMPONENT_ID));
		int pDir = pcc->direction;

		Vec2 pLookDir(0, 0);
		if(pDir == PlayerControlerComponent::P_LEFT ) pLookDir = Vec2(-1, 0);
		if(pDir == PlayerControlerComponent::P_RIGHT) pLookDir = Vec2( 1, 0);
		pLookDir *= 22;

		Boxf box = _obj->worldBox();
		if(box.contains(pPos + pLookDir)) {
			if(!hackDisable.empty()) {
				if(_state->hasDeactivateKey) {
					_state->exec(hackDisable.c_str());
					_state->hasDeactivateKey = false;
					const Sound* useSnd = _state->getLoader().getSound("assets/use.wav");
					_state->game()->sounds()->playSound(useSnd, 0);
					 _state->game()->sounds()->haltSound(_channel);
				}
			}
		}
	}
}

void BotComponent::updateDisabled() {
	_obj->sprite->setTileIndex(direction + 2);
}
