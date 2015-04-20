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

#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <vector>
#include <unordered_map>

#include "collision.h"

#include "image_manager.h"

class Scene;

typedef int Tile;


typedef std::unordered_map<std::string,std::string> EntityData;
int getInt(const EntityData& map, const char* key, int def);


class Level {
private:
	typedef std::vector<EntityData>      EntityVector;
public:
	typedef EntityVector::const_iterator EntityIterator;

public:
	Level(Scene* scene);
	
	bool loadFromJsonFile(const char* tiledMap);
	
	inline unsigned width()   const { return _width; }
	inline unsigned height()  const { return _height; }
	inline unsigned nLayers() const { return _layers; }

	inline const TileMap& tileMap() const { return _tileMap; }
	void setTileMap(const TileMap& tileMap);

	Tile getTile(unsigned x, unsigned y, unsigned layer) const;
	void setTile(unsigned x, unsigned y, unsigned layer, Tile val);

	bool tileCollision(Tile tile) const;
	void setTileCollision(Tile tile, bool collision);

	Boxi tileBounds(const Boxf& box) const;
	Boxf tileBox(unsigned x, unsigned y) const;
	bool collide(unsigned layer, const Boxf& box, CollisionInfo* info = nullptr) const;

	inline EntityIterator entityBegin() const { return _entities.begin(); }
	inline EntityIterator entityEnd()   const { return _entities.end();   }

	// void dumpEntities();

private:
	typedef std::vector<bool> BoolVector;

private:
	Scene* _scene;
	
	std::vector<Tile> _map;
	unsigned _width, _height, _layers;
	
	EntityVector _entities;
	
	TileMap    _tileMap;
	BoolVector _collisionTileSet;

private:
	unsigned index(unsigned x, unsigned y, unsigned z) const;
};

#endif
