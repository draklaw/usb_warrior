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
#include "game_object.h"

#include "scene.h"


Scene::Scene(Game* game)
    : _game(game),
      _level(this),
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


void Scene::addLogicComponent(GameObject* obj, unsigned id, LogicComponent* lcomp) {
	if (_logicMap.size() <= id) {
		_logicMap.resize(id);
	}
	
	_logicMap[id].push_back(lcomp);
}


void Scene::beginUpdate() {
	for(GameObject& obj: _objects) {
		obj._nextUpdate();
	}
}


void Scene::updateLogic(unsigned id) {
	for (LogicComponent* lcomp: _logicMap[id]) {
		lcomp->update();
	}
}


void Scene::beginRender() {
	SDL_TRY(SDL_RenderClear(_game->renderer()));
}


void Scene::endRender() {
	SDL_RenderPresent(_game->renderer());
}


void Scene::render(double interp, Boxi viewBox, Boxi screenBox) {
	Vec2 scale = (  screenBox.sizes().array().template cast<float>()
	              / viewBox.  sizes().array().template cast<float>()).matrix();
//	_game->log("view: ", viewBox.min().transpose(), ", ", viewBox.sizes().transpose());
//	_game->log("screen: ", screenBox.min().transpose(), ", ", screenBox.sizes().transpose());
//	_game->log("scale: ", scale.transpose());

	for(SpriteComponent& sprite: _sprites) {
		unsigned index = std::min(sprite.tileIndex(), sprite.tilemap().nTiles() - 1);
		GameObject* obj = sprite.object();
		if(!sprite.isVisible() || !obj || !obj->isActive() || obj->isDestroyed()) {
			if(!obj || obj->isDestroyed()) {
				_game->log("Try to display a sprite linked to an invalid/destroyed game object");
				sprite.setVisible(false);
			}
			continue;
		}

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
		destRect.w = std::floor(sizes.x() + epsilon);
		destRect.h = std::floor(sizes.y() + epsilon);

//		_game->log("Before: ", destRect.x, ", ", destRect.y, ", ", destRect.w, ", ", destRect.h);
		destRect.x  = (destRect.x - viewBox.min().x()) * scale.x() + screenBox.min().x();
		destRect.y  = (destRect.y - viewBox.min().y()) * scale.y() + screenBox.min().y();
		destRect.w *= scale.x();
		destRect.h *= scale.y();
//		_game->log("After : ", destRect.x, ", ", destRect.y, ", ", destRect.w, ", ", destRect.h);

		// TODO: rotation / flips
		SDL_TRY(SDL_RenderCopy(_game->renderer(),
		                       sprite.tilemap().image()->texture,
		                       &tileRect, &destRect));
	}
}


void Scene::renderLevelLayer(unsigned layer, Boxi viewBox, Boxi screenBox) {

	Vec2 scale = screenBox.sizes().array().template cast<float>()
	           / viewBox.  sizes().array().template cast<float>();

	Vec2i lvlTileSize = _level.tileMap().tileSize().array()
	                  * scale.template cast<int>().array();
	Boxi levelBox(
				Vec2i( viewBox.min().x()    / lvlTileSize.x(),
					   viewBox.min().y()    / lvlTileSize.y()),
				Vec2i((viewBox.max().x()-1) / lvlTileSize.x() + 1,
					  (viewBox.max().y()-1) / lvlTileSize.y() + 1)
	);
	Boxi maxBox = Boxi(Vec2i::Zero(),
	                   Vec2i(_level.width()-1, _level.height()-1));
	levelBox = levelBox.intersection(maxBox);

	SDL_Rect destRect;
	destRect.w = lvlTileSize.x() * scale.x();
	destRect.h = lvlTileSize.y() * scale.y();

	for(int y = levelBox.min().y(); y <= levelBox.max().y(); ++y) {
		for(int x = levelBox.min().x(); x <= levelBox.max().x(); ++x) {
			Tile tile = _level.getTile(x, y, layer);
			if(tile < 0) continue;

			SDL_Rect tileRect = _level.tileMap().tileRect(tile);
			destRect.x = (x * lvlTileSize.x() - viewBox.min().x())
								* scale.x() + screenBox.min().x();
			destRect.y = (y * lvlTileSize.y() - viewBox.min().y())
								* scale.y() + screenBox.min().y();
			SDL_TRY(SDL_RenderCopy(_game->renderer(),
								   _level.tileMap().image()->texture,
								   &tileRect, &destRect));
		}
	}
}
