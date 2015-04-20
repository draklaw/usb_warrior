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

#include "components/move_component.h"
#include "utils.h"
#include "game.h"

#include "test_state.h"


TestState::TestState(Game* game)
	: GameState(game, "Test", durationFromSeconds(UPDATE_TIME)),
	  _scene(game),
	  _loader(game),
	  _input(game),
	  _left(INVALID_INPUT),
	  _right(INVALID_INPUT),
	  _up(INVALID_INPUT),
	  _down(INVALID_INPUT),
	  _use(INVALID_INPUT),
	  _obj(nullptr),
	  _msound(nullptr),
	  _jsound(nullptr),
	  _music(nullptr),
	  _font(nullptr) {
}


void TestState::update() {
	_scene.beginUpdate();

	_input.sync();

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
	
	// moves
	if(_obj->isEnabled()) {
		if(_input.isPressed(_left))  ppm->walk(LEFT);
		if(_input.isPressed(_right)) ppm->walk(RIGHT);
		if(_input.isPressed(_up))    ppm->jump();
//		if(_input.isPressed(_down))  /* TODO: Duck ! */;
	}
	
//	if(_input.justPressed(_use)) _obj->setEnabled(!_obj->isEnabled());
	if(_input.justPressed(_use)) {
		LogicComponent* cmp =  _obj->getComponent(MOVE_COMPONENT_ID);
		cmp->setEnabled(!cmp->isEnabled());
	}

	_scene.updateLogic(MOVE_COMPONENT_ID);

	// sounds
	static bool was_moving = false;
	bool is_moving = _input.isPressed(_left) || _input.isPressed(_right)
		|| _input.isPressed(_up) || _input.isPressed(_down);

	if(is_moving && !was_moving) {
		_mchannel = _game->sounds()->playSound(_msound, -1);
	} else if (!is_moving && was_moving) {
		_game->sounds()->haltSound(_mchannel);
	}
	was_moving = is_moving;

	if(_input.justPressed(_use)) _game->sounds()->playSound(_jsound, 0);
}


void TestState::frame(double interp) {
	Vec2 screenSize = _game->screenSize().template cast<float>();
	Vec2 viewCenter = _obj->posInterp(interp);
	Boxf viewBox(viewCenter - screenSize / 2,
	             viewCenter + screenSize / 2);
	Boxf screenBox(Vec2::Zero(), screenSize);

	_scene.beginRender();

	if(_scene.level().nLayers() > 0) {
		_scene.renderLevelLayer(0, viewBox, screenBox);
	}

	_scene.render(interp, viewBox, screenBox);

	for(unsigned layer = 1; layer < _scene.level().nLayers(); ++layer) {
		_scene.renderLevelLayer(layer, viewBox, screenBox);
	}

	_font.render(_game, 100, 400, "Hello World !");
	_font.render(_game, 100, 432,
	             "A quite long string in a box a bit too small.", 200);

	_scene.endRender();
}


void TestState::initialize() {
	_game->log("Initialize TestState...");

	// ##### TESTS HERE !!! #####

	_loader.addImage("assets/test/tileset.png");
	_loader.addImage("assets/ts_placeholder.png");
	_loader.addImage("assets/test/font_0.png");

	_loader.addSound("assets/test/laser0.wav");
	_loader.addSound("assets/test/laser1.wav");
	_loader.addMusic("assets/test/music.ogg");
	_loader.addFont("assets/test/font.txt");

	_loader.loadAll();

	_left  = _input.addInput("left");
	_right = _input.addInput("right");
	_up    = _input.addInput("jump");
	_down  = _input.addInput("down");
	_use   = _input.addInput("use");

	_input.loadKeyBindingFile("assets/test/keymap.json");

	_input.bindJsonKeys(_left,  "left",  SDL_SCANCODE_LEFT);
	_input.bindJsonKeys(_right, "right", SDL_SCANCODE_RIGHT);
	_input.bindJsonKeys(_up,    "jump",  SDL_SCANCODE_UP);
	_input.bindJsonKeys(_down,  "down",  SDL_SCANCODE_DOWN);
	_input.bindJsonKeys(_use,   "use",   SDL_SCANCODE_SPACE);

	_tilemap = TileMap(_loader.getImage("assets/test/tileset.png"), 32, 32);


	_msound = _loader.getSound("assets/test/laser0.wav");
	_jsound = _loader.getSound("assets/test/laser1.wav");
	_music  = _loader.getMusic("assets/test/music.ogg");
	_mchannel = -1;

	_font = _loader.getFont("assets/test/font.txt");
	_font.setImage(_loader.getImage("assets/test/font_0.png"));

	_scene.level().setTileMap(TileMap(_loader.getImage("assets/ts_placeholder.png"), 32, 32));

	_scene.level().loadFromJsonFile("assets/level_0.json");
	_scene.level().setTileCollision(12, true);
	_scene.level().setTileCollision(13, true);

	_obj = _scene.addObject("Test");
	_scene.addSpriteComponent(_obj, _tilemap, 1);
	_scene.addLogicComponent(_obj, MOVE_COMPONENT_ID, new MoveComponent(_obj));
	_obj->computeBoxFromSprite(Vec2(.5, .5), 1);
	_obj->geom().pos = Vec2(1920/4, 1080/4);
}


void TestState::shutdown() {
	_game->log("Shutdown TestState...");
	_loader.releaseAll();
}


void TestState::start() {
	_game->log("Start TestState...");
// 	_game->log("Play music...");
// 	_game->sounds()->playMusic(_music);
}


void TestState::stop() {
// 	_game->log("Halt music...");
// 	_game->sounds()->haltMusic();
	_game->log("Stop TestState...");
}
