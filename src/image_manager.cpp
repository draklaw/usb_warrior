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

#include <SDL2/SDL_surface.h>

#include <SDL2/SDL_image.h>

#include "game.h"

#include "image_manager.h"


Image::~Image() {
	if(texture) {
		SDL_DestroyTexture(texture);
	}
}


TileMap::TileMap()
	: _image(nullptr),
      _tileSize(Vec2i::Zero()) {
}


TileMap::TileMap(const Image* image, unsigned tileWidth, unsigned tileHeight)
	: _image(image),
      _tileSize(tileWidth, tileHeight) {
}


unsigned TileMap::nHTiles() const {
	return _image->size.x() / _tileSize.x();
}


unsigned TileMap::nVTiles() const {
	return _image->size.y() / _tileSize.y();
}


unsigned TileMap::nTiles() const {
	return nHTiles() * nVTiles();
}


SDL_Rect TileMap::tileRect(unsigned i) const {
	assert(_image);
	SDL_Rect rect;
	unsigned nh = nHTiles();
	rect.x = (i % nh) * _tileSize.x();
	rect.y = (i / nh) * _tileSize.y();
	rect.w = _tileSize.x();
	rect.h = _tileSize.y();
	assert(rect.x + rect.w <= int(_image->size.x()));
	assert(rect.y + rect.h <= int(_image->size.y()));
	return rect;
}


void TileMap::_reset() {
	_image      = nullptr;
	_tileSize   = Vec2i::Zero();
}


ImageManager::ImageManager(Game* game)
    : _game(game),
      _imageMap() {
}


TileMap ImageManager::loadTilemap(const std::string& filename,
                                  unsigned tileWidth, unsigned tileHeight) {
	auto it = _imageMap.find(filename);
	if(it == _imageMap.end()) {
		Image img;

		_game->log("Load image \"", filename, "\"...");

		SDL_Surface* surf = IMG_Load(filename.c_str());
		if(!surf) {
			_game->error("Failed to load image: ", IMG_GetError());
			return TileMap();
		}

		img.texture = SDL_CreateTextureFromSurface(_game->renderer(), surf);
		if(!img.texture) {
			SDL_FreeSurface(surf);
			_game->error("Failed to create \"", filename, "\" texture: ", SDL_GetError());
			return TileMap();
		}

		img.size     = Vec2i(surf->w, surf->h);
		img.name     = filename;
		img.useCount = 0;

		SDL_FreeSurface(surf);

		auto res = _imageMap.emplace(filename, img);
		assert(res.second);
		it = res.first;

		// Do not destroy the texture when img goes out of scope
		img.texture = nullptr;
	}

	++(it->second.useCount);

	return TileMap(&it->second, tileWidth, tileHeight);
}


void ImageManager::releaseTilemap(TileMap& tilemap) {
	auto it = _imageMap.find(tilemap.image()->name);
	assert(it != _imageMap.end());

	tilemap._reset();
	--(it->second.useCount);

	if(!it->second.useCount) {
		_game->log("Release image \"", it->second.name, "\"...");
		_imageMap.erase(it);
	}
}
