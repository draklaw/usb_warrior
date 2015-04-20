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


#include <cassert>

#include "json.h"
#include "game.h"

#include "input.h"


#define FAIL(test) do {			           \
		if (test) {	                       \
			mapScanCode(input, scanCode);  \
			return;                        \
		}	                               \
	} while (false)


InputManager::InputManager(Game* game)
    : _game(game),
	  _bindings(NULL),
      _inputMap(),
      _scanCodeMap() {
}


InputManager::~InputManager() {
	if(_bindings) { json_free_value(&_bindings); }
}


Input InputManager::addInput(const char* name) {
	_inputMap.emplace_back(name);
	return _inputMap.size() - 1;
}


void  InputManager::mapScanCode(Input input, ScanCode scanCode) {
	_scanCodeMap.emplace(scanCode, input);
}


void InputManager::bindJsonKeys(Input input, const char* name, ScanCode scanCode) {
	FAIL(_bindings == NULL);
	// get the array of keys corresponding to 'name'
	json_t* item = json_find_first_label(_bindings, name);
	FAIL(item == NULL || item->child == NULL || item->child->type != JSON_ARRAY);

	// get the first key of that array
	item = item->child->child;
	FAIL(item == NULL);

	do {
		FAIL(item->type != JSON_STRING);
		SDL_Scancode code = SDL_GetScancodeFromName(item->text);
		FAIL(code == SDL_SCANCODE_UNKNOWN);
		mapScanCode(input, code);
		item = item->next;
	} while (item != NULL);
}


void InputManager::loadKeyBindingFile(const char* filename) {
	FILE* jsonFile = fopen(filename, "r");
	if(jsonFile == NULL) { _game->log("Failed to open: ", filename); }

	if(json_stream_parse(jsonFile, &_bindings) == JSON_OK) {
		_game->log("Load keys  \"", filename, "\"...");
	} else {
		_game->log("Failed to parse: \"", filename, "\"");
		_bindings = NULL;
	}
	fclose(jsonFile);
}


void InputManager::sync() {
	for(InputDesc& in: _inputMap) {
		in.prevCount = in.count;
		in.count = 0;
	}

	int keyCount = 0;
	const Uint8* keyState = SDL_GetKeyboardState(&keyCount);
	for(auto keyInput: _scanCodeMap) {
		if(int(keyInput.first) < keyCount) {
			_inputMap[keyInput.second].count += keyState[keyInput.first];
		}
	}
}


bool InputManager::isPressed(Input input) const {
	assert(input < _inputMap.size());
	return _inputMap[input].count;
}

bool InputManager::justPressed(Input input) const {
	const InputDesc& desc = _inputMap[input];
	return desc.count && !desc.prevCount;
}


bool InputManager::justReleased(Input input) const {
	const InputDesc& desc = _inputMap[input];
	return !desc.count && desc.prevCount;
}
