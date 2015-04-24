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

#ifndef _UW_IMAGE_MANAGER_H_
#define _UW_IMAGE_MANAGER_H_


#include <unordered_map>

#include <SDL2/SDL_render.h>

#include "uw_math.h"


class Game;

class ImageManager;


class Image {
public:
	~Image();

	SDL_Texture* texture;
	Vec2i        size;

	std::string  name;
	unsigned     useCount;
};


class TileMap {
public:
	TileMap();
	TileMap(const Image* image, unsigned tileWidth, unsigned tileHeight);

	unsigned nHTiles() const;
	unsigned nVTiles() const;
	unsigned nTiles() const;
	SDL_Rect tileRect(unsigned i) const;

	inline const Image* image()    const { return _image; }
	inline const Vec2i  tileSize() const { return _tileSize; }

	void _reset();

private:
	const Image* _image;
	Vec2i        _tileSize;
};


class ImageManager {
public:
	ImageManager(Game* game);

	const Image* loadImage(const std::string& filename);
	void releaseImage(const Image* img);

private:
	typedef std::unordered_map<std::string, Image> ImageMap;

private:
	Game*    _game;

	ImageMap _imageMap;
};


#endif
