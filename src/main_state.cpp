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


#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_rect.h>

#include <SDL2/SDL_image.h>

#include "utils.h"
#include "game.h"
#include "actions.h"

#include "components/player_controler_component.h"
#include "components/noclip_move_component.h"
#include "components/move_component.h"
#include "components/trigger_component.h"
#include "components/bot_component.h"
#include "components/wall_component.h"

#include "main_state.h"


MainState::MainState(Game* game)
	: GameState(game, "Main", durationFromSeconds(UPDATE_TIME)),
		_scene(game),
		_loader(game),
		_nextLevel(),
		_input(game),
		_left  (INVALID_INPUT),
		_right (INVALID_INPUT),
		_jump  (INVALID_INPUT),
		_up    (INVALID_INPUT),
		_down  (INVALID_INPUT),
		_use   (INVALID_INPUT),
		_debug0(INVALID_INPUT),
		_debug1(INVALID_INPUT),
		_player(nullptr),
		_font() {
}


void MainState::update() {
	if(!_nextLevel.empty()) {
		_scene.level().loadFromJsonFile(_nextLevel.c_str());
		resetLevel();
		_nextLevel.clear();
	}

	_scene.beginUpdate();

	_input.sync();

	if(_input.justPressed(_debug0)) _scene.setDebug(!_scene.debug());
	if(_input.justPressed(_debug1)) {
		auto mc = _player->getComponent(MOVE_COMPONENT_ID);
		auto nmc = _player->getComponent(NOCLIP_MOVE_COMPONENT_ID);
		mc->setEnabled(!mc->isEnabled());
		nmc->setEnabled(!mc->isEnabled());
	}

	for(unsigned compId = 0; compId < COMPONENT_COUNT; ++compId) {
		_scene.updateLogic(compId);
	}
}


void MainState::frame(double interp) {
	Vec2 screenSize = _game->screenSize().template cast<float>();
	Vec2 viewCenter = _player->posInterp(interp);
	Boxf viewBox(viewCenter - screenSize / 2,
	             viewCenter + screenSize / 2);
	Boxf screenBox(Vec2::Zero(), screenSize);

	_scene.beginRender();

	for(unsigned layer = 0; layer < _scene.level().nLayers(); ++layer) {
		_scene.renderLevelLayer(layer, viewBox, screenBox);
	}

	_scene.render(interp, viewBox, screenBox);

	SDL_Renderer* dali = _game->renderer();
	SDL_Rect r1, r2;
	
	/* /!\ WARNING PHAT CODE WARNING PHAT CODE WARNING PHAT CODE WARNING /!\ */
	r1.x = screenSize.x() - 300;
	r1.y = screenSize.y() - 50;
	r1.w = 300;
	r1.h = 50;
	
	SDL_SetRenderDrawColor(dali,200,200,200,255);
	SDL_RenderFillRect(dali,&r1);
	
	r1.x = screenSize.x() - 300;
	r1.y = screenSize.y() - 50 + ((50-32)/2);
	r1.w = 32;
	r1.h = 32;
	
	SDL_Texture* key1 = _loader.getImage("assets/clef1.png")->texture;
	if (!hasDeactivateKey)
		SDL_SetTextureAlphaMod(key1,64);
	SDL_RenderCopy(dali, key1, NULL, &r1);
	SDL_SetTextureAlphaMod(key1,255);
	
	r1.x = r1.x + 32 + 20;
	
	SDL_Texture* key2 = _loader.getImage("assets/clef2.png")->texture;
	if (!hasComputerKey)
		SDL_SetTextureAlphaMod(key2,64);
	SDL_RenderCopy(dali, key2, NULL, &r1);
	SDL_SetTextureAlphaMod(key2,255);
	
	r1.x = r1.x + 32 + 20;
	
	SDL_Texture* key3 = _loader.getImage("assets/clef3.png")->texture;
	if (!hasFightClubKey)
		SDL_SetTextureAlphaMod(key3,64);
	SDL_RenderCopy(dali, key3, NULL, &r1);
	SDL_SetTextureAlphaMod(key3,255);
	
	r1.x = r1.x + 32 + 20;
	
	SDL_Texture* key4 = _loader.getImage("assets/clef4.png")->texture;
	if (!hasMysteryKey)
		SDL_SetTextureAlphaMod(key4,64);
	SDL_RenderCopy(dali, key4, NULL, &r1);
	SDL_SetTextureAlphaMod(key4,255);

	_scene.endRender();
}


GameObject* MainState::getObject(const std::string& name) {
	auto it = _objects.find(name);
	return (it == _objects.end())? nullptr: it->second;
}


void MainState::loadLevel(const char* filename) {
	_nextLevel = filename;
}


void MainState::resetLevel() {
	_objects.clear();
	_scene.clear();
	_player = nullptr;
	hasDeactivateKey = false;
	hasComputerKey = false;
	hasFightClubKey = false;
	hasMysteryKey = false;

	for(Level::EntityIterator entity = _scene.level().entityBegin();
	    entity != _scene.level().entityEnd(); ++entity) {

		GameObject* obj = nullptr;
		const std::string& type = entity->at("type");
		_game->log("create ", entity->at("name"));
		if     (type == "player")     obj = createPlayer   (*entity);
		else if(type == "trigger")    obj = createTrigger  (*entity);
		else if(type == "bot_static") obj = createBotStatic(*entity);
		else if(type == "wall")       obj = createWall     (*entity);

		if(obj) {
			auto ri = _objects.emplace(obj->name(), obj);
			if(!ri.second) {
				_game->warning("Multiple objects with name \"", obj->name(), "\"");
			}
		} else {
			_game->warning("Failed to create object \"", entity->at("name"), "\" of type \"", type, "\"");
		}
	}

	if(!_player) {
		_game->error("Level does not have a player spawn");
		_game->quit();
	}
}


GameObject* MainState::createSpriteObject(const EntityData& data,
                                          const TileMap& tileMap) {
	const std::string& name = getString(data, "name", "");
	GameObject* obj = _scene.addObject(name.empty()? nullptr: name.c_str());
	_scene.addSpriteComponent(obj, tileMap, 0);

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


GameObject* MainState::createPlayer(const EntityData& data) {
	if(_player) {
		_game->warning("Level has several player spawns");
		return nullptr;
	}

	_player = createSpriteObject(data, _playerTileMap);

	auto pcc = new PlayerControlerComponent(this, _player);
	pcc->left  = _left;
	pcc->right = _right;
	pcc->up    = _up;
	pcc->down  = _down;
	pcc->jump  = _jump;
	_scene.addLogicComponent(_player, PLAYER_CONTROLLER_COMPONENT_ID, pcc);

	_scene.addLogicComponent(_player, MOVE_COMPONENT_ID,
	                         new MoveComponent(_player));

	auto nmc = new NoclipMoveComponent(this, _player);
	nmc->left  = _left;
	nmc->right = _right;
	nmc->up    = _up;
	nmc->down  = _down;
	nmc->setEnabled(false);
	_scene.addLogicComponent(_player, NOCLIP_MOVE_COMPONENT_ID, nmc);

	return _player;
}


GameObject* MainState::createTrigger(const EntityData& data) {
	const Image* img = _loader.getImage(getString(data, "sprite", ""));
	int tileX = getInt(data, "tiles_x", 2);
	int tileY = getInt(data, "tiles_y", 1);

	GameObject* obj = createSpriteObject(data,
	             TileMap(img, img->size.x() / tileX, img->size.y() / tileY));
	auto ec = new TriggerComponent(this, obj);
	ec->setEnabled(getInt(data, "enabled", true));
	ec->tileEnable      = getInt   (data, "tile_enable", 0);
	ec->tileDisable     = getInt   (data, "tile_disable", 1);
	ec->animCount       = getInt   (data, "anim_count", 1);
	ec->animSpeed       = getInt   (data, "anim_speed", 60);
	ec->hitPoint        = getString(data, "hit_point", "");
	ec->pointCoords.x() = getInt   (data, "point_x", 0);
	ec->pointCoords.y() = getInt   (data, "point_y", 0);
	ec->hit             = getString(data, "hit", "");
	ec->use             = getString(data, "use", "");

	_scene.addLogicComponent(obj, TRIGGER_COMPONENT_ID, ec);
	return obj;
}


GameObject* MainState::createBotStatic(const EntityData& data) {
	const Image* img = _loader.getImage("assets/toutrobot.png");
	GameObject* obj = createSpriteObject(data, TileMap(img, 32, 48));

	auto bc = new BotComponent(this, obj);
	bc->direction   = getInt   (data, "direction", 0);
	bc->fov         = getInt   (data, "fov", 30);
	bc->seePlayer   = getString(data, "see_player", "");
	bc->hackDisable = getString(data, "hack_disable", "");
	_scene.addLogicComponent(obj, BOT_COMPONENT_ID, bc);

	return obj;
}


GameObject* MainState::createWall(const EntityData& data) {
	const std::string& name = getString(data, "name", "");
	GameObject* obj = _scene.addObject(name.empty()? nullptr: name.c_str());

	float x = getInt(data, "x",      0);
	float y = getInt(data, "y",      0);
	float w = getInt(data, "width",  0);
	float h = getInt(data, "height", 0);
	obj->geom().pos = Vec2(x, y);
	obj->geom().box = Boxf(Vec2(0, 0), Vec2(w, h));

	auto wc = new WallComponent(this, obj);
	wc->setEnabled(getInt(data, "enabled", true));
	_scene.addLogicComponent(obj, WALL_COMPONENT_ID, wc);

	return obj;
}


void MainState::addCommand(const char* action, Command cmd) {
	_commandMap.emplace(action, cmd);
}


void MainState::exec(const char* cmd) {
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


void MainState::initialize() {
	_game->log("Initialize MainState...");

	_left   = _input.addInput("left");
	_right  = _input.addInput("right");
	_jump   = _input.addInput("jump");
	_up     = _input.addInput("up");
	_down   = _input.addInput("down");
	_use    = _input.addInput("use");
	_debug0 = _input.addInput("debug0");
	_debug1 = _input.addInput("debug1");

	_input.loadKeyBindingFile("assets/keymap.json");

	_input.bindJsonKeys(_left,  "left",  SDL_SCANCODE_LEFT);
	_input.bindJsonKeys(_right, "right", SDL_SCANCODE_RIGHT);
	_input.bindJsonKeys(_jump,  "jump",  SDL_SCANCODE_E);
	_input.bindJsonKeys(_up,    "up",    SDL_SCANCODE_UP);
	_input.bindJsonKeys(_down,  "down",  SDL_SCANCODE_DOWN);
	_input.bindJsonKeys(_use,   "use",   SDL_SCANCODE_SPACE);
	_input.mapScanCode(_debug0, SDL_SCANCODE_F1);
	_input.mapScanCode(_debug1, SDL_SCANCODE_F2);

	// Loading
	_loader.addImage("assets/tilez.png");
	_loader.addImage("assets/toutAMI.png");
	_loader.addImage("assets/toutrobot.png");
	_loader.addImage("assets/exit.png");
	_loader.addImage("assets/terminal.png");
	_loader.addImage("assets/alarm.png");
	_loader.addImage("assets/clef1.png");
	_loader.addImage("assets/clef2.png");
	_loader.addImage("assets/clef3.png");
	_loader.addImage("assets/clef4.png");

	_loader.loadAll();

	// ##### Level
	_scene.level().setTileMap(TileMap(_loader.getImage("assets/tilez.png"), 16, 16));

	// Walls
	for(unsigned i = 0; i < 8; ++i) {
		_scene.level().setTileCollision(0   + i, true);
		_scene.level().setTileCollision(64  + i, true);
		_scene.level().setTileCollision(128 + i, true);
		_scene.level().setTileCollision(192 + i, true);
		_scene.level().setTileCollision(320 + i, true);
		_scene.level().setTileCollision(384 + i, true);
		_scene.level().setTileCollision(448 + i, true);
		_scene.level().setTileCollision(512 + i, true);
		_scene.level().setTileCollision(576 + i, true);
	}
	// Platforms
	for(unsigned i = 0; i < 3; ++i) {
		_scene.level().setTileCollision(256 + i, true);
	}
	for(unsigned i = 0; i < 12; ++i) {
		_scene.level().setTileCollision( 8 + i * 64, true);
		_scene.level().setTileCollision( 9 + i * 64, true);
		_scene.level().setTileCollision(10 + i * 64, true);
		_scene.level().setTileCollision(11 + i * 64, true);
	}
	_scene.level().setTileCollision(774, true);
	_scene.level().setTileCollision(838, true);

	// ##### TileMaps
	_playerTileMap = TileMap(_loader.getImage("assets/toutAMI.png"), 32, 48);
	_exitTileMap   = TileMap(_loader.getImage("assets/exit.png"), 64, 64);

	// Action !
	addCommand("load_level", loadLevelAction);
	addCommand("echo",       echoAction);
	addCommand("enable",     enableAction);
	addCommand("disable",    disableAction);
	addCommand("add_item",   addItemAction);
	addCommand("set_state",  setStateAction);

	loadLevel("assets/level2.json");
}


void MainState::shutdown() {
	_game->log("Shutdown MainState...");
	_loader.releaseAll();
}


void MainState::start() {
	_game->log("Start MainState...");
}


void MainState::stop() {
	_game->log("Stop MainState...");
}
