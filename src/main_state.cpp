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

#include "main_state.h"

#include "components/move_component.h"
#include "utils.h"
#include "game.h"


MainState::MainState(Game* game)
    : GameState(game, "Main", durationFromSeconds(UPDATE_TIME)),
      _scene(game),
      _loader(game),
      _input(game),
      _left(INVALID_INPUT),
      _right(INVALID_INPUT),
      _up(INVALID_INPUT),
      _down(INVALID_INPUT),
      _use(INVALID_INPUT),
      _obj(nullptr) {
}


void MainState::update() {
	_scene.beginUpdate();
	
	_input.sync();
	
	if(_input.justPressed(_left))  _game->sounds()->playSound(_sounds[0]);
	if(_input.justPressed(_right)) _game->sounds()->playSound(_sounds[1]);
	if(_input.justPressed(_up))    _game->sounds()->playSound(_sounds[2]);
	if(_input.justPressed(_down))  _game->sounds()->playSound(_sounds[3]);

	// ppm <=> Player Puppet Master
	MoveComponent* ppm = static_cast<MoveComponent*>(_obj->getComponent(MOVE_COMPONENT_ID));

//	Vec2i tileSize = _scene.level().tileMap().tileSize();
//	Tile tile = _scene.level().getTile(geom.pos.x() / tileSize.x(),
//	                                   geom.pos.y() / tileSize.y(), 0);
//	bool coll = _scene.level().tileCollision(tile);
	CollisionInfo info;
	Boxf objBox = _obj->worldBox();
	bool coll = _scene.level().collide(0, objBox, &info);
	_obj->sprite->setTileIndex(coll? 0: 1);
	if(coll) {
		_game->log("Collision: ", info.flags, " - ", info.penetration.transpose());
	}
	
	if(_obj->isActive()) {
		if(_input.isPressed(_left))  ppm->walk(LEFT);
		if(_input.isPressed(_right)) ppm->walk(RIGHT);
		if(_input.isPressed(_up))    ppm->jump();
		if(_input.isPressed(_down))  /* TODO: Duck ! */;
	}
	
	if(_input.justPressed(_use))
		_obj->setActive(!_obj->isActive());
	
	_scene.updateLogic(MOVE_COMPONENT_ID);
}


void MainState::frame(double interp) {
	Vec2i viewCenter = _obj->geom().pos.template cast<int>();
	Boxi viewBox(viewCenter - _game->screenSize() / 2,
	             viewCenter + _game->screenSize() / 2);
	Boxi screenBox(Vec2i::Zero(), _game->screenSize());

	_scene.beginRender();

	if(_scene.level().nLayers() > 0) {
		_scene.renderLevelLayer(0, viewBox, screenBox);
	}

	_scene.render(interp, viewBox, screenBox);

	for(unsigned layer = 1; layer < _scene.level().nLayers(); ++layer) {
		_scene.renderLevelLayer(layer, viewBox, screenBox);
	}

	_scene.endRender();
}


void MainState::initialize() {
	_game->log("Initialize MainState...");

	_loader.addImage("assets/test/tileset.png");
	_loader.addImage("assets/ts_placeholder.png");

	_loader.addSound("assets/test/laser0.wav");
	_loader.addSound("assets/test/laser1.wav");
	_loader.addSound("assets/test/laser2.wav");
	_loader.addSound("assets/test/laser3.wav");
	_loader.addMusic("assets/test/music.ogg");

	_loader.loadAll();

	_left  = _input.addInput("left");
	_right = _input.addInput("right");
	_up    = _input.addInput("up");
	_down  = _input.addInput("down");
	_use   = _input.addInput("use");

	_input.mapScanCode(_left,  SDL_SCANCODE_LEFT);
	_input.mapScanCode(_right, SDL_SCANCODE_RIGHT);
	_input.mapScanCode(_up,    SDL_SCANCODE_UP);
	_input.mapScanCode(_down,  SDL_SCANCODE_DOWN);
	_input.mapScanCode(_use,   SDL_SCANCODE_SPACE);

	_tilemap = TileMap(_loader.getImage("assets/test/tileset.png"), 32, 32);

	_scene.level().setTileMap(TileMap(_loader.getImage("assets/ts_placeholder.png"), 32, 32));

	_scene.level().loadFromJsonFile("assets/level_0.json");
	_scene.level().setTileCollision(12, true);
	_scene.level().setTileCollision(13, true);

	_sounds[0] = _loader.getSound("assets/test/laser0.wav");
	_sounds[1] = _loader.getSound("assets/test/laser1.wav");
	_sounds[2] = _loader.getSound("assets/test/laser2.wav");
	_sounds[3] = _loader.getSound("assets/test/laser3.wav");
	_music     = _loader.getMusic("assets/test/music.ogg");

	_obj = _scene.addObject("Test");
	_scene.addSpriteComponent(_obj, _tilemap, 1);
	_scene.addLogicComponent(_obj, MOVE_COMPONENT_ID, new MoveComponent(_obj));
	_obj->computeBoxFromSprite(Vec2(.5, .5), 1);
	_obj->geom().pos = Vec2(1920/4, 1080/4);
}


void MainState::shutdown() {
	_game->log("Shutdown MainState...");

	_loader.releaseAll();
}


void MainState::start() {
	_game->log("Start MainState...");
// 	_game->log("Play music...");
// 	_game->sounds()->playMusic(_music);
}


void MainState::stop() {
// 	_game->log("Halt music...");
// 	_game->sounds()->haltMusic();
	_game->log("Stop MainState...");
}
