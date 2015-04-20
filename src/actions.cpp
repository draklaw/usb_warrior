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
#include "main_state.h"

#include "actions.h"


void loadLevelAction(MainState* state, unsigned argc, const char** argv) {
	if(argc != 2) {
		state->game()->warning("Invalid loadLevel call");
	}

	state->game()->warning("loadLevelAction: ", argc, ", ", argv[1]);
	state->loadLevel(argv[1]);
}


void echoAction(MainState* state, unsigned argc, const char** argv) {
	printf("Echo: <");
	if(argc > 1) printf("%s", argv[1]);
	for(unsigned i = 2; i < argc; i++)
		printf(",%s", argv[i]);
	printf(">\n");
}


void _enableHelper(MainState* state, const std::string& compName,
                   const std::string& objName, bool enable) {
	int comp = -1;
	if(compName == "object")  comp = -1;
	if(compName == "trigger") comp = TRIGGER_COMPONENT_ID;
	else {
		state->game()->warning("En/Disable action: Invalid component ", compName);
		return;
	}

	GameObject* obj = state->getObject(objName);
	if(!obj) {
		state->game()->warning("En/Disable action: Invalid object ", objName);
		return;
	}

	if(comp == -1) {
		obj->setEnabled(false);
	} else {
		obj->getComponent(comp)->setEnabled(enable);
	}
}

void enableAction(MainState* state, unsigned argc, const char** argv) {
	if(argc != 3) {
		state->game()->warning("Enable action: Invalid call");
	}
	_enableHelper(state, argv[1], argv[2], true);
}


void disableAction(MainState* state, unsigned argc, const char** argv) {
	if(argc != 3) {
		state->game()->warning("Disable action: Invalid call");
	}
	_enableHelper(state, argv[1], argv[2], false);
}

void addItemAction(MainState* state, unsigned argc, const char** argv) {
	if(argc != 2) {
		state->game()->warning("add_item action: Invalid call");
	}
	std::string type = argv[1];
	if(type == "deactivate") {
		state->hasDeactivateKey = true;
	} else {
		state->game()->warning("add_item action: invalid key type");
	}
}
