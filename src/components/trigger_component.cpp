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

#include <cstdio>

#include "../game.h"
#include "../main_state.h"
#include "../game_object.h"
#include "../scene.h"
#include "../input.h"

#include "sprite_component.h"

#include "trigger_component.h"


TriggerComponent::TriggerComponent(Scene* scene, GameObject* obj)
    : Component(scene, obj) {
}


void TriggerComponent::update() {
	_obj->sprite->setTileIndex(
	            tileEnable + (_animCounter / animSpeed) % animCount);

	GameObject* tgtObj = _scene->getByName(target);
	if(!tgtObj) return;
	Boxf pBox = tgtObj->worldBox();
	Boxf wBox = _obj->worldBox();
	if(!hitPoint.empty()) {
		Vec2 point = wBox.min() + pointCoords;
		if(pBox.contains(point)) {
			_scene->exec(hitPoint.c_str());
		}
	}

	if(!hit.empty()) {
		if(!pBox.intersection(wBox).isEmpty()) {
			_scene->exec(hit.c_str());
		}
	}

	Input* useInput = _scene->inputs()->getByName("use");
	bool doUse = useInput && useInput->justPressed();
	if(!use.empty() && doUse) {
		Vec2 usePoint = pBox.center();
		if(wBox.contains(usePoint)) {
			_scene->exec(use.c_str());
		}
	}

	++_animCounter;
}


void TriggerComponent::updateDisabled() {
	_obj->sprite->setTileIndex(tileDisable);
}
