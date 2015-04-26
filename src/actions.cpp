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


#include "game.h"
#include "sound_player.h"
#include "loader.h"
#include "scene.h"

#include "main_state.h"
#include "credit_state.h"

#include "components/trigger_component.h"
#include "components/bot_component.h"
#include "components/wall_component.h"

#include "actions.h"


void loadLevelAction(Scene* scene, unsigned argc, const char** argv) {
	if(argc != 2) {
		scene->game()->warning("Invalid loadLevel call");
	}

	scene->game()->warning("loadLevelAction: ", argc, ", ", argv[1]);
	// FIXME: create a good mechanic for level loading.
	scene->loadLevel(argv[1]);
}


void echoAction(Scene* /*scene*/, unsigned argc, const char** argv) {
	printf("Echo: <");
	if(argc > 1) printf("%s", argv[1]);
	for(unsigned i = 2; i < argc; i++)
		printf(",%s", argv[i]);
	printf(">\n");
}


void _enableHelper(Scene* scene, const std::string& compName,
                   const std::string& objName, bool enable) {
	GameObject* obj = scene->getByName(objName);
	if(!obj) {
		scene->game()->warning("En/Disable action: Invalid object ", objName);
		return;
	}

	Component* comp = nullptr;
	if(compName == "object") {
		obj->setEnabled(enable);
		return;
	}
	else if(compName == "trigger")    comp = obj->trigger;
	else if(compName == "bot_static") comp = obj->bot;
	else if(compName == "wall")       comp = obj->wall;
	else {
		scene->game()->warning("En/Disable action: Invalid component ", compName);
		return;
	}

	if(!comp) {
		scene->game()->warning("En/Disable action: Object ", objName, " do not have component ", compName);
		return;
	}
	comp->setEnabled(enable);
}


void enableAction(Scene* scene, unsigned argc, const char** argv) {
	if(argc != 3) {
		scene->game()->warning("Enable action: Invalid call");
	}
	_enableHelper(scene, argv[1], argv[2], true);
}


void disableAction(Scene* scene, unsigned argc, const char** argv) {
	if(argc != 3) {
		scene->game()->warning("Disable action: Invalid call");
	}
	_enableHelper(scene, argv[1], argv[2], false);
}


void addItemAction(Scene* scene, unsigned argc, const char** argv) {
	if(argc != 2) {
		scene->game()->warning("add_item action: Invalid call");
	}
	std::string type = argv[1];
	if(type == "deactivate") {
		scene->state()->hasDeactivateKey = true;
		const Sound* snd = scene->loader()->getSound("assets/loot.wav");
		scene->game()->sounds()->playSound(snd, 0);
	} else {
		scene->game()->warning("add_item action: invalid key type");
	}
}


void setStateAction(Scene* scene, unsigned argc, const char** argv) {
	if(argc != 2) {
		scene->game()->warning("set_state action: Invalid call");
	}
	std::string next = argv[1];
	if(next == "credits") {
		scene->game()->creditState()->image = "assets/credits.png";
		scene->game()->creditState()->titleScreen = false;
		scene->game()->changeState(scene->game()->creditState());
		scene->state()->quit();
	}
}
