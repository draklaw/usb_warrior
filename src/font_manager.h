/*
 *  Copyright (C) 2015 the authors
 *
 *  This file is part of usb_warrior.
 *
 *  usb_warrior is free software: you can redistribute it and/or modify
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

#ifndef _FONT_H_
#define _FONT_H_


#include <unordered_map>

#include "image_manager.h"


class Game;


struct Character {
	int x;
	int y;
	int width;
	int height;
	int xoff;
	int yoff;
	int xadv;
};


class Font {
public:
	Font();

	bool parseFile(const char* filename);

	void setImage(const Image* image);

	void render(Game* game, unsigned x, unsigned y, char* text,
	            unsigned maxWidth = 0xffffffff);

public:
	std::string  name;
	unsigned     useCount;

private:
	typedef std::unordered_map<unsigned, Character> CharMap;

private:
	int          _lineHeight;
	int          _baseLine;

	CharMap      _charMap;
	const Image  _image;
};


class FontManager {
public:
	FontManager(Game* game);

	const Font* loadFont(const std::string& filename);
	void releaseFont(const Font* font);

private:
	typedef std::unordered_map<std::string, Font> FontMap;

private:
	Game*   _game;

	FontMap _fontMap;
};

#endif
