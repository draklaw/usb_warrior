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


Input* InputManager::addInput(const std::string& name) {
	_inputMap.emplace_back(new Input(name));
	Input* ptr = _inputMap.back().get();
	_inputNameMap.emplace(name, ptr);
	return ptr;
}


Input* InputManager::getByName(const std::string& name) {
	auto it = _inputNameMap.find(name);
	return (it == _inputNameMap.end())?
		nullptr:
		it->second;
}


void InputManager::mapScanCode(Input* input, ScanCode scanCode) {
	_scanCodeMap[scanCode].push_back(input);
}


void InputManager::bindJsonKeys(Input* input, const char* name, ScanCode scanCode) {
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
	// FIXME: If an input is pressed between two updates, it may be missed.
	for(auto& input: _inputMap) {
		input->prevCount = input->count;
		input->count     = 0;
	}

	int keyCount = 0;
	const Uint8* keyState = SDL_GetKeyboardState(&keyCount);
	for(auto& keyInputList: _scanCodeMap) {
		for(Input* input: keyInputList.second) {
			input->count += keyState[keyInputList.first];
		}
	}
}
