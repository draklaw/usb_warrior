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


#include <sstream>

#include "utils.h"
#include "game.h"

#include "scene.h"


Scene::Scene(Game* game)
    : _game(game),
      _objectCounter(0),
      _objects() {
}

GameObject* Scene::addObject(const char* name) {
	if(!name) {
		std::ostringstream out;
		out << "object_" << _objectCounter;
		return addObject(out.str().c_str());
	} else {
		_game->log("Create object \"", name, "\"");
		_objects.emplace_back(this, name);
		++_objectCounter;
		return &_objects.back();
	}
}

void Scene::addSpriteComponent(GameObject* obj, const TileMap& tilemap,
                               unsigned index) {
	if(obj->sprite) {
		_game->warning("Object \"", obj->name(), "\" has already a SpriteComponent. Do nothing.");
		return;
	}
	_sprites.emplace_back(obj, tilemap, index);
	obj->sprite = &_sprites.back();
}


void Scene::beginUpdate() {
	for(GameObject& obj: _objects) {
		obj._nextUpdate();
	}
}


void Scene::render(double interp) {
	SDL_TRY(SDL_RenderClear(_game->renderer()));
	for(SpriteComponent& sprite: _sprites) {
		unsigned index = std::min(sprite.tileIndex(), sprite.tilemap().nTiles() - 1);
		GameObject* obj = sprite.object();
		if(!obj) {
			continue;
		}
		assert(!obj->isDestroyed());

		const GeometryComponent& geom0 = obj->geom(1);
		const GeometryComponent& geom1 = obj->geom(0);

		float epsilon = .001;
		Vec2 pos     = lerp<Vec2>(interp, geom0.pos,         geom1.pos);
		//float rot   = lerp(interp, geom0.rot,               geom1.rot);
		Vec2 topleft = lerp<Vec2>(interp, geom0.box.min(),   geom1.box.min());
		Vec2 sizes   = lerp<Vec2>(interp, geom0.box.sizes(), geom1.box.sizes());

		SDL_Rect tileRect = sprite.tilemap().tileRect(index);
		SDL_Rect destRect;
		destRect.x = std::floor(pos.x() + topleft.x() - epsilon);
		destRect.y = std::floor(pos.y() + topleft.y() - epsilon);
		destRect.w = std::floor(sizes.x() - epsilon);
		destRect.h = std::floor(sizes.y() - epsilon);

		// TODO: rotation / flips
		SDL_TRY(SDL_RenderCopy(_game->renderer(),
		                       sprite.tilemap().image()->texture,
					           &tileRect, &destRect));
	}
	SDL_RenderPresent(_game->renderer());
}
