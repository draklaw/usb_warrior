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

typedef int Tile;

class Level {
public:
	Level();
	~Level();
	
	bool loadFromJsonFile(const char* tiledMap);
	
	Tile getTile(unsigned x, unsigned y, unsigned layer);
	void setTile(unsigned x, unsigned y, unsigned layer, Tile val);
	
	// void dumpEntities();

private:
	typedef std::unordered_map<std::string,std::string> EntityData;

private:
	std::vector<Tile> _map;
	unsigned _width, _height, _layers;
	
	std::vector<EntityData> _entities;

private:
	unsigned index(unsigned x, unsigned y, unsigned z);
};

#endif
