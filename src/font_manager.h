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


class Character {
public:
	SDL_Rect charRect() const;
	SDL_Rect destRect(unsigned x, unsigned y) const;

public:
	int _x;
	int _y;
	int _w;
	int _h;
	int _xoff;
	int _yoff;
	int _xadv;
};


class FontImpl {
public:
	bool parseFile(const char* filename);

	void render(Game* game, const Image* image, unsigned x, unsigned y,
	            const char* text, unsigned maxWidth) const;
public:
	std::string  name;
	unsigned     useCount;

private:
	typedef std::unordered_map<unsigned, Character> CharMap;

private:
	int     _lineHeight;
	int     _baseLine;

	CharMap _charMap;
};


class Font {
public:
	Font(const FontImpl* font);

	void setImage(const Image* image);

	void render(Game* game, unsigned x, unsigned y, const char* text,
	            unsigned maxWidth = 0xffffffff) const;

private:
	const FontImpl* _font;
	const Image*    _image;
};


class FontManager {
public:
	FontManager(Game* game);

	const FontImpl* loadFont(const std::string& filename);
	void releaseFont(const FontImpl* font);

private:
	typedef std::unordered_map<std::string, FontImpl> FontMap;

private:
	Game*   _game;

	FontMap _fontMap;
};

#endif
