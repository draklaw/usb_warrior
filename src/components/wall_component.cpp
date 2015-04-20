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

#include "wall_component.h"



class MainState;

WallComponent::WallComponent(MainState* state, GameObject* obj)
    : LogicComponent(obj),
      _state(state),
      _prevEnabled(false) {
}

void WallComponent::update() {
	if(!_prevEnabled) {
		setTiles(true);
	}

	_prevEnabled = true;
}

void WallComponent::updateDisabled() {
	if(_prevEnabled) {
		setTiles(false);
	}

	_prevEnabled = false;
}

void WallComponent::setTiles(bool enable) {
	Level& level = _obj->scene()->level();
	Vec2 tileSize = level.tileMap().tileSize().template cast<float>();
	Vec2i first = ((_obj->geom().pos.array() / tileSize.array()).matrix() + Vec2(.5, .5))
					.template cast<int>();
	int height = _obj->geom().box.sizes().y() / tileSize.y() + .5;

	_state->game()->log("setTiles(", enable, "): ", first.transpose(), ", ", height);

	Tile tile = enable? 774: 655;
	for(unsigned i = 0; i < height; ++i) {
		level.setTile(first.x(), first.y() + i, 0, tile);
		if(tile == 774) tile = 838;
	}
}
