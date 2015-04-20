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

#include "components/player_controler_component.h"
#include "components/move_component.h"

#include "main_state.h"


MainState::MainState(Game* game)
	: GameState(game, "Main", durationFromSeconds(UPDATE_TIME)),
	  _scene(game),
	  _loader(game),
	  _input(game),
	  _left (INVALID_INPUT),
	  _right(INVALID_INPUT),
	  _jump (INVALID_INPUT),
	  _use  (INVALID_INPUT),
      _player(nullptr),
      _font() {
}


void MainState::update() {
	_scene.beginUpdate();

	_input.sync();

	if(_input.justPressed(_debug)) _scene.setDebug(!_scene.debug());

	_scene.updateLogic(PLAYER_CONTROLLER_COMPONENT_ID);
	_scene.updateLogic(MOVE_COMPONENT_ID);
}


void MainState::frame(double interp) {
	Vec2 screenSize = _game->screenSize().template cast<float>();
	Vec2 viewCenter = _player->posInterp(interp);
	Boxf viewBox(viewCenter - screenSize / 2,
	             viewCenter + screenSize / 2);
	Boxf screenBox(Vec2::Zero(), screenSize);

	_scene.beginRender();

	if(_scene.level().nLayers() > 0) {
		_scene.renderLevelLayer(0, viewBox, screenBox);
	}

	_scene.render(interp, viewBox, screenBox);

	_scene.endRender();
}


void MainState::loadLevel(const char* filename) {
	_scene.level().loadFromJsonFile(filename);
	resetLevel();
}


void MainState::resetLevel() {
	_objects.clear();
	_scene.clear();
	_player = nullptr;

	for(Level::EntityIterator entity = _scene.level().entityBegin();
	    entity != _scene.level().entityEnd(); ++entity) {

		const std::string& type = entity->at("type");
		if     (type == "player")     createPlayer   (*entity);
		else if(type == "exit")       createExit     (*entity);
		else if(type == "tp")         createTP       (*entity);
		else if(type == "bot_static") createBotStatic(*entity);
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

	for(auto& kv: data) {
		_game->log("  ", kv.first, ": ", kv.second);
	}

	_game->log("Exit: ", x, ", ", y, ", ", w, ", ", h);
	_game->log("Exit pos: ", obj->geom().pos.transpose());

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
	pcc->jump  = _jump;
	_scene.addLogicComponent(_player, PLAYER_CONTROLLER_COMPONENT_ID, pcc);
	_scene.addLogicComponent(_player, MOVE_COMPONENT_ID,
	                         new MoveComponent(_player));

	return _player;
}


GameObject* MainState::createExit(const EntityData& data) {
	GameObject* obj = createSpriteObject(data, _exitTileMap);

	return obj;
}


GameObject* MainState::createTP(const EntityData& data) {
}


GameObject* MainState::createBotStatic(const EntityData& data) {
}


void MainState::initialize() {
	_game->log("Initialize MainState...");

	_left  = _input.addInput("left");
	_right = _input.addInput("right");
	_jump  = _input.addInput("jump");
	_use   = _input.addInput("use");
	_debug = _input.addInput("debug");

	_input.mapScanCode(_left,  SDL_SCANCODE_LEFT);
	_input.mapScanCode(_right, SDL_SCANCODE_RIGHT);
	_input.mapScanCode(_jump,  SDL_SCANCODE_UP);
	_input.mapScanCode(_use,   SDL_SCANCODE_SPACE);
	_input.mapScanCode(_debug, SDL_SCANCODE_F1);

	_loader.addImage("assets/tilez.png");
	_loader.addImage("assets/test/character.png");
	_loader.addImage("assets/exit.png");

	_loader.loadAll();

	// ##### Level
	_scene.level().setTileMap(TileMap(_loader.getImage("assets/tilez.png"), 16, 16));

	// Walls
	for(unsigned i = 0; i < 8; ++i) {
		_scene.level().setTileCollision(0   + i, true);
		_scene.level().setTileCollision(64  + i, true);
		_scene.level().setTileCollision(128 + i, true);
		_scene.level().setTileCollision(192 + i, true);
	}
	// Platforms
	for(unsigned i = 0; i < 3; ++i) {
		_scene.level().setTileCollision(256 + i, true);
	}

	// ##### TileMaps
	_playerTileMap = TileMap(_loader.getImage("assets/test/character.png"), 32, 64);
	_exitTileMap   = TileMap(_loader.getImage("assets/exit.png"), 64, 64);

	loadLevel("assets/level1.json");
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
