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

#include "input.h"


InputManager::InputManager(Game* game)
    : _game(game),
      _inputMap(),
      _scanCodeMap() {
}


Input InputManager::addInput(const char* name) {
	_inputMap.emplace_back(name);
	return _inputMap.size() - 1;
}


void  InputManager::mapScanCode(Input input, ScanCode scanCode) {
	_scanCodeMap.emplace(scanCode, input);
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
