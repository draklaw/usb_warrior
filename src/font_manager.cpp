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

#include <cstdio>

#include "game.h"
#include "image_manager.h"

#include "font_manager.h"


SDL_Rect Character::charRect() const {
	SDL_Rect rect;
	rect.x = _x;
	rect.y = _y;
	rect.w = _w;
	rect.h = _h;
	return rect;
}


SDL_Rect Character::destRect(unsigned x, unsigned y) const {
	SDL_Rect rect;
	rect.x = x + _xoff;
	rect.y = y + _yoff;
	rect.w = _w;
	rect.h = _h;
	return rect;
}


Font::Font(const FontImpl* font)
    : _font(font),
      _image(nullptr) {
}


void Font::setImage(const Image* image) {
	_image = image;
}


void Font::render(Game* game, unsigned x, unsigned y, const char* text,
                  unsigned maxWidth /* = 0xffffffff */) const {
	_font->render(game, _image, x, y, text, maxWidth);
}


bool FontImpl::parseFile(const char *filename) {
	FILE* fd = fopen(filename, "r");
	if (fd == NULL) { return false; }

	if(fscanf(fd, "%d %d %*d %*d %*d %*d %*d %*d %*d %*d\n",
	          &_lineHeight, &_baseLine) != 2) {	goto fail; }

	int nb_chars;
	if(fscanf(fd, "%d\n", &nb_chars) != 1) { goto fail; }
	for (int i = 0; i < nb_chars; i++) {
		unsigned codepoint;
		Character chr;
		if(fscanf(fd, "%u %d %d %d %d %d %d %d %*d %*d\n", &codepoint,
		          &chr._x, &chr._y, &chr._w, &chr._h, &chr._xoff, &chr._yoff,
		          &chr._xadv) != 8) { goto fail; }
		_charMap.emplace(codepoint, chr);
	}

	fclose(fd);
	return true;

fail:
	fclose(fd);
	return false;
}


void FontImpl::render(Game* game, const Image* image, unsigned x, unsigned y,
                      const char* text, unsigned maxWidth /* = 0xffffffff */) const {
	unsigned xOrig = x;
	unsigned len = std::strlen(text);
	const Character* chars[len];
	
	unsigned i = 0;
	while(text[i] == ' ') { i++; }
	unsigned firstCol = i;
	unsigned lastSpace = i;

	while(i < len) {
		if(text[i] == ' ') { lastSpace = i; }
		auto chrIt = _charMap.find(text[i]);
		if(chrIt == _charMap.end()) { chrIt = _charMap.find('?'); }
		chars[i] = &(chrIt->second);
		if(maxWidth < 0xffffffff && x + chars[i]->_xadv > xOrig + maxWidth) {
			if(lastSpace == firstCol) { lastSpace = i; }
			renderLine(game, image, xOrig, y, chars, firstCol, lastSpace);
			if(i != lastSpace) { i = lastSpace + 1; }
			while(text[i] == ' ') { i++; }
			lastSpace = firstCol = i;
			x = xOrig;
			y += _baseLine;
			continue;
		}
		x += chars[i]->_xadv;
		i++;
	}
	renderLine(game, image, xOrig, y, chars, firstCol, len);
}


void FontImpl::renderLine(Game* game, const Image* image, unsigned x, unsigned y,
	                      const Character** chars, unsigned from, unsigned to) const {
	for(unsigned i = from; i < to; i++) {
		SDL_Rect charRect = chars[i]->charRect();
		SDL_Rect destRect = chars[i]->destRect(x, y);
		SDL_RenderCopy(game->renderer(), image->texture, &charRect, &destRect);
		x += chars[i]->_xadv;
	}
}


FontManager::FontManager(Game* game)
	: _game(game),
	  _fontMap() {
}


const FontImpl* FontManager::loadFont(const std::string& filename) {
	auto it = _fontMap.find(filename);
	if(it == _fontMap.end()) {
		FontImpl fnt;

		_game->log("Load font  \"", filename, "\"...");

		if(!fnt.parseFile(filename.c_str())) {
			_game->error("Failed to load font");
			return nullptr;
		}
		fnt.name = filename;
		fnt.useCount = 0;

		auto res = _fontMap.emplace(filename, fnt);
		assert(res.second);
		it = res.first;
	}

	++(it->second.useCount);

	return &(it->second);
}


void FontManager::releaseFont(const FontImpl* font) {
	auto it = _fontMap.find(font->name);
	assert(it != _fontMap.end());

	--(it->second.useCount);
	if(!it->second.useCount) {
		_game->log("Release font  \"", it->second.name, "\"...");
		_fontMap.erase(it);
	}
}
