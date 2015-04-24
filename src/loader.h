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

#ifndef _UW_LOADER_H_
#define _UW_LOADER_H_

#include <string>
#include <unordered_map>

#include "font_manager.h"


class Game;

class Image;
class Sound;
class Music;
class FontImpl;


class Loader {
public:
	Loader(Game* game);

	void addImage(const std::string& filename);
	void addSound(const std::string& filename);
	void addMusic(const std::string& filename);
	void addFont(const std::string& filename);

	const Image* getImage(const std::string& filename);
	const Sound* getSound(const std::string& filename);
	const Music* getMusic(const std::string& filename);
	      Font   getFont (const std::string& filename);

	unsigned loadAll();
	void releaseAll();

private:
	typedef std::unordered_map<std::string, const Image*> ImageMap;
	typedef std::unordered_map<std::string, const Sound*> SoundMap;
	typedef std::unordered_map<std::string, const Music*> MusicMap;
	typedef std::unordered_map<std::string, const FontImpl*>  FontMap;

private:
	Game*    _game;

	ImageMap _imageMap;
	SoundMap _soundMap;
	MusicMap _musicMap;
	FontMap  _fontMap;
};

#endif
