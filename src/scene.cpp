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
      _debugView(false),
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
		_objects.emplace_back(new GameObject(this, name));
		++_objectCounter;
		return _objects.back().get();
	}
}

void Scene::addSpriteComponent(GameObject* obj, const TileMap& tilemap,
                               unsigned index) {
	if(obj->sprite) {
		_game->warning("Object \"", obj->name(), "\" has already a SpriteComponent. Do nothing.");
		return;
	}
	_sprites.emplace_back(new SpriteComponent(obj, tilemap, index));
	obj->sprite = _sprites.back().get();
}


void Scene::addLogicComponent(GameObject* obj, unsigned id, LogicComponent* lcomp) {
	if (_logicMap.size() <= id) {
		_logicMap.resize(id+1);
	}
	
	_logicMap[id].emplace_back(lcomp);
	obj->_registerLogic(id, lcomp);
}


void Scene::clear() {
	_objectCounter = 0;
	_objects.clear();
	_sprites.clear();
	for(auto& logics: _logicMap) {
		logics.clear();
	}
}


void Scene::setDebug(bool debug) {
	_debugView = debug;
}


void Scene::beginUpdate() {
	for(ObjectPtr& obj: _objects) {
		obj->_nextUpdate();
	}
}


void Scene::updateLogic(unsigned id) {
	if(_logicMap.size() <= id) return;
	for (LogicPtr& lcomp: _logicMap[id]) {
		if(lcomp->object()->isEnabled() && lcomp->isEnabled()) {
			lcomp->update();
		}
	}
}


void Scene::beginRender() {
	SDL_SetRenderDrawColor(_game->renderer(), 0, 0, 0, 255);
	SDL_TRY(SDL_RenderClear(_game->renderer()));
}


void Scene::endRender() {
	SDL_RenderPresent(_game->renderer());
}


void Scene::render(double interp, Boxf viewBox, Boxf screenBox) {
	Vec2 scale = (  screenBox.sizes().array().template cast<float>()
	              / viewBox.  sizes().array().template cast<float>()).matrix();
//	_game->log("view: ", viewBox.min().transpose(), ", ", viewBox.sizes().transpose());
//	_game->log("screen: ", screenBox.min().transpose(), ", ", screenBox.sizes().transpose());
//	_game->log("scale: ", scale.transpose());

	for(SpritePtr& sprite: _sprites) {
		unsigned index = std::min(sprite->tileIndex(), sprite->tilemap().nTiles() - 1);
		GameObject* obj = sprite->object();
		if(!sprite->isVisible() || !obj || !obj->isEnabled() || obj->isDestroyed()) {
			if(!obj || obj->isDestroyed()) {
				_game->log("Try to display a sprite linked to an invalid/destroyed game object");
				sprite->setVisible(false);
			}
			continue;
		}

		float epsilon = .001;
		//float rot   = lerp(interp, geom0.rot,               geom1.rot);
		Boxf box     = obj->worldBoxInterp(interp);
//		_game->log("box: ", box.min().transpose(), ", ", box.sizes().transpose());

		SDL_Rect tileRect = sprite->tilemap().tileRect(index);
		SDL_Rect destRect;
		destRect.x = (box.min().x() - viewBox.min().x()) * scale.x()
		           + screenBox.min().x() - epsilon;
		destRect.y = (box.min().y() - viewBox.min().y()) * scale.y()
		           + screenBox.min().y() - epsilon;
		destRect.w = box.sizes().x() * scale.x() + epsilon;
		destRect.h = box.sizes().y() * scale.y() + epsilon;
//		_game->log("After : ", destRect.x, ", ", destRect.y, ", ", destRect.w, ", ", destRect.h);

		// TODO: rotation / flips
		SDL_TRY(SDL_RenderCopy(_game->renderer(),
		                       sprite->tilemap().image()->texture,
		                       &tileRect, &destRect));

		if(_debugView) {
			SDL_SetRenderDrawColor(_game->renderer(), 0, 255, 0, 255);
			SDL_RenderDrawRect(_game->renderer(), &destRect);
		}
	}
}


void Scene::renderLevelLayer(unsigned layer, Boxf viewBox, Boxf screenBox) {

	Vec2 scale = screenBox.sizes().array() / viewBox.sizes().array();

	Vec2 lvlTileSize = _level.tileMap().tileSize().template cast<float>().array()
	                 * scale.array();
	Boxi boundBox = _level.tileBounds(viewBox);

	SDL_Rect destRect;
	for(int y = boundBox.min().y(); y <= boundBox.max().y(); ++y) {
		for(int x = boundBox.min().x(); x <= boundBox.max().x(); ++x) {
			Tile tile = _level.getTile(x, y, layer);
			if(tile < 0) continue;

			// It is important to compute both corners the same way to avoid
			// roundoff errors.
			Vec2 tileCoord = Vec2(x  , y  ).array() * lvlTileSize.array()
			               - viewBox.min().array();
			Vec2 nextCoord = Vec2(x+1, y+1).array() * lvlTileSize.array()
			               - viewBox.min().array();
			float epsilon = 0.01;
			Vec2 tl = tileCoord.array() * scale.array() + screenBox.min().array();
			Vec2 br = nextCoord.array() * scale.array() + screenBox.min().array();
			destRect.x = tl.x() - epsilon;
			destRect.y = tl.y() - epsilon;
			destRect.w = br.x() - tl.x() + epsilon;
			destRect.h = br.y() - tl.y() + epsilon;

			SDL_Rect tileRect = _level.tileMap().tileRect(tile);
			SDL_TRY(SDL_RenderCopy(_game->renderer(),
								   _level.tileMap().image()->texture,
								   &tileRect, &destRect));
		}
	}
}
