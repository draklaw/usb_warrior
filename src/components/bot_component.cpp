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
#include "../sound_player.h"
#include "../input.h"
#include "../loader.h"
#include "../scene.h"
#include "../game_object.h"
#include "../main_state.h"

#include "sprite_component.h"
#include "player_controler_component.h"

#include "bot_component.h"



class MainState;

BotComponent::BotComponent(Scene* scene, GameObject* obj)
    : Component(scene, obj),
	  _channel(-1)  {
}

void BotComponent::update() {
	_obj->sprite->setTileIndex(direction);

	Vec2 lookDir = Vec2::UnitX();
	if(!direction) lookDir *= -1;

	GameObject* player = _scene->getByName("player");

	Vec2 pos = _obj->geom().pos;
	Vec2 pPos = player->geom().pos;

	if(!seePlayer.empty()) {
		float cosAlpha = (pPos - pos).normalized().dot(lookDir);
		float alpha = std::acos(cosAlpha) / M_PI * 180;

		if(alpha < fov) {
			_scene->exec(seePlayer.c_str());
			if(_channel == -1) {
				const Sound* alarmSnd = _scene->state()->loader()->getSound("assets/alarm.wav");
				_channel = _scene->state()->game()->sounds()->playSound(alarmSnd, -1);
			}
		}
	}

	if(_scene->state()->useInput()->justPressed()) {
		auto pcc = player->playerControler;
		int pDir = pcc->direction;

		Vec2 pLookDir(0, 0);
		if(pDir == PlayerControlerComponent::P_LEFT ) pLookDir = Vec2(-1, 0);
		if(pDir == PlayerControlerComponent::P_RIGHT) pLookDir = Vec2( 1, 0);
		pLookDir *= 22;

		Boxf box = _obj->worldBox();
		if(box.contains(pPos + pLookDir)) {
			if(!hackDisable.empty()) {
				if(_scene->state()->hasDeactivateKey) {
					_scene->exec(hackDisable.c_str());
					_scene->state()->hasDeactivateKey = false;
					const Sound* useSnd = _scene->state()->loader()->getSound("assets/use.wav");
					_scene->state()->game()->sounds()->playSound(useSnd, 0);
					_scene->state()->game()->sounds()->haltSound(_channel);
				}
			}
		}
	}
}

void BotComponent::updateDisabled() {
	_obj->sprite->setTileIndex(direction + 2);
}
