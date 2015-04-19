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
#include "image_manager.h"

#include "loader.h"


Loader::Loader(Game* game) : _game(game) {
}


void Loader::addImage(const std::string& filename) {
	_imageMap.emplace(filename, nullptr);
}


void Loader::addSound(const std::string& filename) {
	_soundMap.emplace(filename, nullptr);
}


void Loader::addMusic(const std::string& filename) {
	_musicMap.emplace(filename, nullptr);
}


void Loader::addFont(const std::string& filename) {
	_fontMap.emplace(filename, nullptr);
}


const Image* Loader::getImage(const std::string& filename) {
	return _imageMap.at(filename);
}


const Sound* Loader::getSound(const std::string& filename) {
	return _soundMap.at(filename);
}


const Music* Loader::getMusic(const std::string& filename) {
	return _musicMap.at(filename);
}


Font Loader::getFont(const std::string& filename) {
	return Font(_fontMap.at(filename));
}


unsigned Loader::loadAll() {
	unsigned err = 0;
	for(auto& file: _imageMap) {
		file.second = _game->images()->loadImage(file.first);
		if(!file.second) { err++; }
	}
	for(auto& file: _soundMap) {
		file.second = _game->sounds()->loadSound(file.first);
		if(!file.second) { err++; }
	}
	for(auto& file: _musicMap) {
		file.second = _game->sounds()->loadMusic(file.first);
		if(!file.second) { err++; }
	}
	for(auto& file: _fontMap) {
		file.second = _game->fonts()->loadFont(file.first);
		if(!file.second) { err++; }
	}
	return err;
}


void Loader::releaseAll() {
	for(auto& file: _imageMap) {
		if(file.second) {
			_game->images()->releaseImage(file.second);
			file.second = nullptr;
		}
	}
	for(auto& file: _soundMap) {
		if(file.second) {
			_game->sounds()->releaseSound(file.second);
			file.second = nullptr;
		}
	}
	for(auto& file: _musicMap) {
		if(file.second) {
			_game->sounds()->releaseMusic(file.second);
			file.second = nullptr;
		}
	}
	for(auto& file: _fontMap) {
		if(file.second) {
			_game->fonts()->releaseFont(file.second);
			file.second = nullptr;
		}
	}
}
