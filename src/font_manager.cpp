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

#include "font_manager.h"


bool Font::parseFile(const char *filename) {
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
		          &chr.x, &chr.y, &chr.width, &chr.height,
		          &chr.xoff, &chr.yoff, &chr.xadv) != 8) { goto fail; }
		_charMap.emplace(codepoint, chr);
	}

	fclose(fd);
	return true;

fail:
	fclose(fd);
	return false;
}


FontManager::FontManager(Game* game)
	: _game(game),
	  _fontMap() {
	
}


const Font* FontManager::loadFont(const std::string& filename) {
	auto it = _fontMap.find(filename);
	if(it == _fontMap.end()) {
		Font fnt;

		_game->log("Load font  \"", filename, "\"...");

		if(!fnt.parseFile(filename.c_str())) {
			_game->error("Failed to load font");
			return new Font();
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


void FontManager::releaseFont(const Font* font) {
	auto it = _fontMap.find(font->name);
	assert(it != _fontMap.end());

	--(it->second.useCount);
	if(!it->second.useCount) {
		_game->log("Release font  \"", it->second.name, "\"...");
		_fontMap.erase(it);
	}
}
