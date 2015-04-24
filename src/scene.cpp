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
#include "level.h"
#include "game.h"
#include "game_object.h"
#include "loader.h"
#include "main_state.h"

#include "components/sprite_component.h"
#include "components/player_controler_component.h"
#include "components/noclip_move_component.h"
#include "components/move_component.h"
#include "components/trigger_component.h"
#include "components/bot_component.h"
#include "components/wall_component.h"

#include "scene.h"


Scene::Scene(Game* game, MainState* state)
    : _game(game),
      _state(state),
      _debugView(false),
      _level(new Level(this)),
      _objectCounter(0),
      _objects() {
	_objects.reserve(SCENE_ARRAYS_MAX_SIZE);
}


GameObject* Scene::getByName(const std::string& name) {
	auto it = _objectsByName.find(name);
	return (it == _objectsByName.end())? nullptr: it->second;
}


GameObject* Scene::addObject(const char* name) {
	// TODO: Remove this when we lift the limitation.
	if(_objects.size() == _objects.capacity()) {
		_game->error("Failed to add object: size limitation reached");
		return nullptr;
	}

	if(!name) {
		std::ostringstream out;
		out << "object_" << _objectCounter;
		return addObject(out.str().c_str());
	} else {
//		_game->log("Create object \"", name, "\"");
		_objects.emplace_back(this, name);
		++_objectCounter;
		return &_objects.back();
	}
}


void Scene::loadLevel(const char* filename) {
	clear();
	_level->loadFromJsonFile(filename);

	for(Level::EntityIterator entity = _level->entityBegin();
	    entity != _level->entityEnd(); ++entity) {

		GameObject* obj = nullptr;
		const std::string& type = entity->at("type");
		_game->log("create ", entity->at("name"));
		if     (type == "player")     obj = createPlayer   (*entity);
		else if(type == "trigger")    obj = createTrigger  (*entity);
		else if(type == "bot_static") obj = createBotStatic(*entity);
		else if(type == "wall")       obj = createWall     (*entity);

		if(obj) {
			auto ri = _objectsByName.emplace(obj->name(), obj);
			if(!ri.second) {
				_game->warning("Multiple objects with name \"", obj->name(), "\"");
			}
		} else {
			_game->warning("Failed to create object \"", entity->at("name"), "\" of type \"", type, "\"");
		}
	}
}


GameObject* Scene::createSpriteObject(const EntityData& data,
                                      const TileMap& tileMap) {
	const std::string& name = getString(data, "name", "");
	GameObject* obj = addObject(name.empty()? nullptr: name.c_str());
	addComponent<SpriteComponent>(obj, tileMap, 0);

	obj->computeBoxFromSprite(Vec2(.5, .5));
	float x = getInt(data, "x",      0);
	float y = getInt(data, "y",      0);
	float w = getInt(data, "width",  0);
	float h = getInt(data, "height", 0);
	obj->geom().pos = Vec2(x + w/2, y + h/2);

//	for(auto& kv: data) {
//		_game->log("  ", kv.first, ": ", kv.second);
//	}

	return obj;
}


GameObject* Scene::createPlayer(const EntityData& data) {
	TileMap tileMap(_state->loader()->getImage("assets/toutAMI.png"), 32, 48);
	GameObject* obj = createSpriteObject(data, tileMap);

	auto pcc = addComponent<PlayerControlerComponent>(obj);
	pcc->left  = _state->_left;
	pcc->right = _state->_right;
	pcc->up    = _state->_up;
	pcc->down  = _state->_down;
	pcc->jump  = _state->_jump;

	addComponent<MoveComponent>(obj);

	auto nmc = addComponent<NoclipMoveComponent>(obj);
	nmc->left  = _state->_left;
	nmc->right = _state->_right;
	nmc->up    = _state->_up;
	nmc->down  = _state->_down;
	nmc->setEnabled(false);

	return obj;
}


GameObject* Scene::createTrigger(const EntityData& data) {
	const Image* img = _state->loader()->getImage(getString(data, "sprite", ""));
	int tileX = getInt(data, "tiles_x", 2);
	int tileY = getInt(data, "tiles_y", 1);

	TileMap tileMap(img, img->size.x() / tileX, img->size.y() / tileY);
	GameObject* obj = createSpriteObject(data, tileMap);

	auto ec = addComponent<TriggerComponent>(obj);
	ec->setEnabled(getInt(data, "enabled", true));
	ec->target          = "player";
	ec->tileEnable      = getInt   (data, "tile_enable", 0);
	ec->tileDisable     = getInt   (data, "tile_disable", 1);
	ec->animCount       = getInt   (data, "anim_count", 1);
	ec->animSpeed       = getInt   (data, "anim_speed", 60);
	ec->hitPoint        = getString(data, "hit_point", "");
	ec->pointCoords.x() = getInt   (data, "point_x", 0);
	ec->pointCoords.y() = getInt   (data, "point_y", 0);
	ec->hit             = getString(data, "hit", "");
	ec->use             = getString(data, "use", "");

	return obj;
}


GameObject* Scene::createBotStatic(const EntityData& data) {
	const Image* img = _state->loader()->getImage("assets/toutrobot.png");
	GameObject* obj = createSpriteObject(data, TileMap(img, 32, 48));

	auto bc = addComponent<BotComponent>(obj);
	bc->direction   = getInt   (data, "direction", 0);
	bc->fov         = getInt   (data, "fov", 30);
	bc->seePlayer   = getString(data, "see_player", "");
	bc->hackDisable = getString(data, "hack_disable", "");

	return obj;
}


GameObject* Scene::createWall(const EntityData& data) {
	const std::string& name = getString(data, "name", "");
	GameObject* obj = addObject(name.empty()? nullptr: name.c_str());

	float x = getInt(data, "x",      0);
	float y = getInt(data, "y",      0);
	float w = getInt(data, "width",  0);
	float h = getInt(data, "height", 0);
	obj->geom().pos = Vec2(x, y);
	obj->geom().box = Boxf(Vec2(0, 0), Vec2(w, h));

	auto wc = addComponent<WallComponent>(obj);
	wc->setEnabled(getInt(data, "enabled", true));

	return obj;
}


void Scene::clear() {
	_objectCounter = 0;
	_objects.clear();
	_objectsByName.clear();
	for(auto& typeComps: _compsMap) {
		typeComps.second.clear();
	}
}


void Scene::setDebug(bool debug) {
	_debugView = debug;
}


void Scene::addCommand(const char* action, Command cmd) {
	_commandMap.emplace(action, cmd);
}


void Scene::exec(const char* cmd) {
//	_game->log("exec: ", cmd);
	std::string line(cmd);

	auto c   = line.begin();
	auto end = line.end();

	std::vector<const char*> argv;
	while (c != end) {
		argv.clear();
		while(c != end && *c != ';') {
			while(c != end && *c != ';' && std::isspace(*c)) ++c;
			if(c == end || *c == ';') break;
			argv.push_back(&*c);
			while(c != end && *c != ';' && !std::isspace(*c)) ++c;
			if(c == end || *c == ';') break;
			if(c != end) *(c++) = '\0';
		}
		if(c != end) *(c++) = '\0';
		while(c != end && *c == ';' && std::isspace(*c)) ++c;

		if(argv.size() == 0) continue;

		_game->lognr("exec:");
		for(unsigned i = 0; i < argv.size(); ++i) {
			_game->lognr(" ", argv[i]);
		}
		_game->log();

		auto pair = _commandMap.find(argv[0]);
		if(pair == _commandMap.end()) {
			_game->warning("Action not found: ", argv[0]);
			continue;
		}

		pair->second(this, argv.size(), argv.data());
	}
}


void Scene::beginUpdate() {
	for(GameObject& obj: _objects) {
		obj._nextUpdate();
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
	SDL_SetRenderDrawBlendMode(_game->renderer(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(_game->renderer(), 0, 255, 0, 64);

	Vec2 scale = (  screenBox.sizes().array().template cast<float>()
	              / viewBox.  sizes().array().template cast<float>()).matrix();
//	_game->log("view: ", viewBox.min().transpose(), ", ", viewBox.sizes().transpose());
//	_game->log("screen: ", screenBox.min().transpose(), ", ", screenBox.sizes().transpose());
//	_game->log("scale: ", scale.transpose());

	auto& spritesComps = _compsMap[std::type_index(typeid(SpriteComponent))];
//	for(SpritePtr& sprite: iter<SpriteComponent>()) {
	for(CompPtr& comp: spritesComps) {
		SpriteComponent* sprite = static_cast<SpriteComponent*>(comp.get());
		unsigned index = std::min(sprite->tileIndex(), sprite->tilemap().nTiles() - 1);
		GameObject* obj = sprite->object();
		if(!sprite->isEnabled() || !obj || !obj->isEnabled() || obj->isDestroyed()) {
			if(!obj || obj->isDestroyed()) {
				_game->log("Try to display a sprite linked to an invalid/destroyed game object");
				sprite->setEnabled(false);
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
			SDL_RenderFillRect(_game->renderer(), &destRect);
		}
	}
}


void Scene::renderLevelLayer(unsigned layer, Boxf viewBox, Boxf screenBox) {
	SDL_SetRenderDrawBlendMode(_game->renderer(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(_game->renderer(), 255, 0, 0, 64);

	Vec2 scale = screenBox.sizes().array() / viewBox.sizes().array();

	Vec2 lvlTileSize = _level->tileMap().tileSize().template cast<float>().array()
	                 * scale.array();
	Boxi boundBox = _level->tileBounds(viewBox);

	SDL_Rect destRect;
	for(int y = boundBox.min().y(); y <= boundBox.max().y(); ++y) {
		for(int x = boundBox.min().x(); x <= boundBox.max().x(); ++x) {
			Tile tile = _level->getTile(x, y, layer);
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

			SDL_Rect tileRect = _level->tileMap().tileRect(tile);
			SDL_TRY(SDL_RenderCopy(_game->renderer(),
								   _level->tileMap().image()->texture,
								   &tileRect, &destRect));

			if(_debugView && _level->tileCollision(tile)) {
				SDL_RenderFillRect(_game->renderer(), &destRect);
			}
		}
	}
}
