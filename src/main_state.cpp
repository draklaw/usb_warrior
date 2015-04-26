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
#include "level.h"
#include "scene.h"
#include "loader.h"
#include "font_manager.h"
#include "sound_player.h"
#include "input.h"

#include "components/sprite_component.h"
#include "components/player_controler_component.h"
#include "components/noclip_move_component.h"
#include "components/move_component.h"
#include "components/trigger_component.h"
#include "components/bot_component.h"
#include "components/wall_component.h"

#include "main_state.h"


MainState::MainState(Game* game)
	: GameState(game, "Main", durationFromSeconds(UPDATE_TIME)),
		_scene    (new Scene(game, this)),
		_loader   (new Loader(game)),
		_level    (),
		_reload   (false),
		_player   (nullptr) {
}


MainState::~MainState() {
}


void MainState::update() {
	if(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_ESCAPE]) {
		quit();
		_game->quit();
	}
	if(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_F3]) {
		_game->setFullscreen(true);
	}
	if(SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_F4]) {
		_game->setFullscreen(false);
	}

	if(_reload) {
		loadLevelNow(_level.c_str());
	}

	_scene->beginUpdate();
	_scene->inputs()->sync();

	if(_scene->inputs()->getByName("debug0")->justPressed()) {
		_scene->setDebug(!_scene->debug());
	}
	if(_scene->inputs()->getByName("debug1")->justPressed()) {
		auto mc = _player->move;
		auto nmc = _player->noclipMove;
		mc->setEnabled(!mc->isEnabled());
		nmc->setEnabled(!mc->isEnabled());
	}

	_scene->forEachComponent<PlayerControlerComponent>(
	            [](PlayerControlerComponent* comp) {
		if(comp->isEnabled() && comp->object()->isEnabled()) {
			comp->update();
		}
	});
	_scene->forEachComponent<NoclipMoveComponent>(
	            [](NoclipMoveComponent* comp) {
		if(comp->isEnabled() && comp->object()->isEnabled()) {
			comp->update();
		}
	});
	_scene->forEachComponent<MoveComponent>(
	            [](MoveComponent* comp) {
		if(comp->isEnabled() && comp->object()->isEnabled()) {
			comp->update();
		}
	});
	_scene->forEachComponent<TriggerComponent>(
	            [](TriggerComponent* comp) {
		if(comp->isEnabled() && comp->object()->isEnabled()) {
			comp->update();
		} else {
			comp->updateDisabled();
		}
	});
	_scene->forEachComponent<BotComponent>(
	            [](BotComponent* comp) {
		if(comp->isEnabled() && comp->object()->isEnabled()) {
			comp->update();
		} else {
			comp->updateDisabled();
		}
	});
	_scene->forEachComponent<WallComponent>(
	            [](WallComponent* comp) {
		if(comp->isEnabled() && comp->object()->isEnabled()) {
			comp->update();
		} else {
			comp->updateDisabled();
		}
	});
}


void MainState::frame(double interp) {
	Vec2 screenSize = _game->screenSize().template cast<float>();
	Boxf screenBox(Vec2::Zero(), screenSize);

	Vec2 levelSize(_scene->level()->width()  * _scene->level()->tileMap().tileSize().x(),
	               _scene->level()->height() * _scene->level()->tileMap().tileSize().y());
	Boxf levelBox(Vec2::Zero(), levelSize);
	Vec2 levelCenter = levelBox.center();

	Boxf centerBox((levelBox.min() + screenSize / 2).cwiseMin(levelCenter),
	               (levelBox.max() - screenSize / 2).cwiseMax(levelCenter));

	Vec2 viewCenter = _player->posInterp(interp);
	if(!centerBox.contains(viewCenter)) {
		viewCenter = viewCenter.cwiseMax(centerBox.min()).cwiseMin(centerBox.max());
	}
	Boxf viewBox(viewCenter - screenSize / 2,
	             viewCenter + screenSize / 2);

	_scene->beginRender();

	for(unsigned layer = 0; layer < _scene->level()->nLayers(); ++layer) {
		_scene->renderLevelLayer(layer, viewBox, screenBox);
	}

	_scene->render(interp, viewBox, screenBox);

	SDL_Renderer* dali = _game->renderer();
	SDL_Rect r1;
	
	/* /!\ WARNING PHAT CODE WARNING PHAT CODE WARNING PHAT CODE WARNING /!\ */
	r1.x = screenSize.x() - 300;
	r1.y = screenSize.y() - 50;
	r1.w = 300;
	r1.h = 50;
	
	SDL_SetRenderDrawColor(dali,200,200,200,255);
	SDL_RenderFillRect(dali,&r1);
	
	r1.x = screenSize.x() - 300 + ((50-32)/2);
	r1.y = screenSize.y() - 50 + ((50-32)/2);
	r1.w = 32;
	r1.h = 32;
	
	SDL_Texture* key1 = _loader->getImage("assets/clef1.png")->texture;
	if (!hasDeactivateKey)
		SDL_SetTextureAlphaMod(key1,64);
	SDL_RenderCopy(dali, key1, NULL, &r1);
	SDL_SetTextureAlphaMod(key1,255);
	
	r1.x = r1.x + 32 + 20;
	
	SDL_Texture* key2 = _loader->getImage("assets/clef2.png")->texture;
	if (!hasComputerKey)
		SDL_SetTextureAlphaMod(key2,64);
	SDL_RenderCopy(dali, key2, NULL, &r1);
	SDL_SetTextureAlphaMod(key2,255);
	
	r1.x = r1.x + 32 + 20;
	
	SDL_Texture* key3 = _loader->getImage("assets/clef3.png")->texture;
	if (!hasFightClubKey)
		SDL_SetTextureAlphaMod(key3,64);
	SDL_RenderCopy(dali, key3, NULL, &r1);
	SDL_SetTextureAlphaMod(key3,255);
	
	r1.x = r1.x + 32 + 20;
	
	SDL_Texture* key4 = _loader->getImage("assets/clef4.png")->texture;
	if (!hasMysteryKey)
		SDL_SetTextureAlphaMod(key4,64);
	SDL_RenderCopy(dali, key4, NULL, &r1);
	SDL_SetTextureAlphaMod(key4,255);

	_scene->endRender();
}


void MainState::loadLevelNow(const char* level) {
	_scene->loadLevel(level);
	_reload = false;
	_player = _scene->getByName("player");
	assert(_player);
}


void MainState::initialize() {
	_game->log("Initialize MainState...");

	Input* left   = _scene->inputs()->addInput("left");
	Input* right  = _scene->inputs()->addInput("right");
	Input* jump   = _scene->inputs()->addInput("jump");
	Input* up     = _scene->inputs()->addInput("up");
	Input* down   = _scene->inputs()->addInput("down");
	Input* use    = _scene->inputs()->addInput("use");
	Input* debug0 = _scene->inputs()->addInput("debug0");
	Input* debug1 = _scene->inputs()->addInput("debug1");

	_scene->inputs()->loadKeyBindingFile("assets/keymap.json");

	_scene->inputs()->bindJsonKeys(left,  "left",  SDL_SCANCODE_LEFT);
	_scene->inputs()->bindJsonKeys(right, "right", SDL_SCANCODE_RIGHT);
	_scene->inputs()->bindJsonKeys(jump,  "jump",  SDL_SCANCODE_E);
	_scene->inputs()->bindJsonKeys(up,    "up",    SDL_SCANCODE_UP);
	_scene->inputs()->bindJsonKeys(down,  "down",  SDL_SCANCODE_DOWN);
	_scene->inputs()->bindJsonKeys(use,   "use",   SDL_SCANCODE_SPACE);
	_scene->inputs()->mapScanCode(debug0, SDL_SCANCODE_F1);
	_scene->inputs()->mapScanCode(debug1, SDL_SCANCODE_F2);

	// Loading
	_loader->addImage("assets/tilez.png");
	_loader->addImage("assets/toutAMI.png");
	_loader->addImage("assets/toutrobot.png");
	_loader->addImage("assets/exit.png");
	_loader->addImage("assets/terminal.png");
	_loader->addImage("assets/alarm.png");
	_loader->addImage("assets/clef1.png");
	_loader->addImage("assets/clef2.png");
	_loader->addImage("assets/clef3.png");
	_loader->addImage("assets/clef4.png");
	_loader->addImage("assets/tp.png");

	_loader->addSound("assets/use.wav");
	_loader->addSound("assets/loot.wav");
	_loader->addSound("assets/alarm.wav");

	_loader->addMusic("assets/niveau.wav");

	_loader->loadAll();

	// ##### Level
	_scene->level()->setTileMap(TileMap(_loader->getImage("assets/tilez.png"), 16, 16));

	// Walls
	for(unsigned i = 0; i < 8; ++i) {
		_scene->level()->setTileCollision(0   + i, true);
		_scene->level()->setTileCollision(64  + i, true);
		_scene->level()->setTileCollision(128 + i, true);
		_scene->level()->setTileCollision(192 + i, true);
		_scene->level()->setTileCollision(320 + i, true);
		_scene->level()->setTileCollision(384 + i, true);
		_scene->level()->setTileCollision(448 + i, true);
		_scene->level()->setTileCollision(512 + i, true);
		_scene->level()->setTileCollision(576 + i, true);
	}
	// Platforms
	for(unsigned i = 0; i < 3; ++i) {
		_scene->level()->setTileCollision(256 + i, true);
	}
	for(unsigned i = 0; i < 12; ++i) {
		_scene->level()->setTileCollision( 8 + i * 64, true);
		_scene->level()->setTileCollision( 9 + i * 64, true);
		_scene->level()->setTileCollision(10 + i * 64, true);
		_scene->level()->setTileCollision(11 + i * 64, true);
	}
	_scene->level()->setTileCollision(773, true);
	_scene->level()->setTileCollision(774, true);
	_scene->level()->setTileCollision(838, true);

	// Action !
	_scene->addCommand("load_level", loadLevelAction);
	_scene->addCommand("echo",       echoAction);
	_scene->addCommand("enable",     enableAction);
	_scene->addCommand("disable",    disableAction);
	_scene->addCommand("add_item",   addItemAction);
	_scene->addCommand("set_state",  setStateAction);

	loadLevelNow("assets/level2.json");
}


void MainState::shutdown() {
	_game->log("Shutdown MainState...");
	_loader->releaseAll();
}


void MainState::start() {
	_game->log("Start MainState...");
	_game->sounds()->playMusic(_loader->getMusic("assets/niveau.wav"));
}


void MainState::stop() {
	_game->log("Stop MainState...");
	_game->sounds()->haltMusic();
}
