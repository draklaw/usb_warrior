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

 #include "image_loader.h"


Loader::Loader(Game* game) : _game(game) {
}


void Loader::addImage(const std::string& filename) {
	_imageMap.emplace(filename, nullptr);
}


void Loader::addSound(const std::string& filename) {
	_soundMap.emplace(filename, nullptr);
}


unsigned Loader::loadAllImages() {
	unsigned err = 0;
	for(auto& fileImg: _imageMap) {
		fileImg.second = _game->images()->load(fileImg.first);
		if(!fileImg.second) { err++; }
	}
	return err;
}


unsigned Loader::loadAllSounds() {
	unsigned err = 0;
	for(auto& fileSnd: _soundMap) {
		fileSnd.second = _game->sounds()->load(fileSnd.first);
		if(!fileSnd.second) { err++; }
	}
	return err;
}


void Loader::releaseAllImages() {
	for(auto& fileImg: _imageMap) {
		if(fileImg.second) {
			_game->images()->release(fileImg.first);
			fileImg.second = nullptr;
		}
	}
}


void Loader::releaseAllSounds() {
	for(auto& fileSnd: _soundMap) {
		if(fileSnd.second) {
			_game->sounds()->release(fileSnd.first);
			fileSnd.second = nullptr;
		}
	}
}
