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

#include <cstdlib>
#include <cstdio>

#include <iostream>

#include "level.h"
#include "json/json.h"

#define PANIC(test) do {               \
	if (test) {                          \
		printf (#test ": %i\n", __LINE__); \
		goto panic;                        \
	}                                    \
} while (false)

Level::Level()
{
	_width = _height = _layers = 0;
}

bool Level::loadFromJsonFile (const char* tiledMap)
{
	json_t* root = NULL;
	json_t* item = NULL;
	json_t* iter = NULL;
	
	FILE* jsonFile = fopen(tiledMap, "r");
	PANIC(jsonFile == NULL);
	
	json_stream_parse (jsonFile, &root);
	fclose(jsonFile);
	
	// Checking width number.
	item = json_find_first_label(root,"width");
	PANIC(item == NULL || item->child == NULL || item->child->type != JSON_NUMBER);
	_width = atoi(item->child->text);
	
	// Checking height number.
	item = json_find_first_label(root,"height");
	PANIC(item == NULL || item->child == NULL || item->child->type != JSON_NUMBER);
	_height = atoi(item->child->text);
	
	// Checking layers array.
	item = json_find_first_label(root,"layers");
	PANIC(item == NULL || item->child == NULL || item->child->type != JSON_ARRAY);
	item = item->child;
	
	// Counting layers.
	iter = item->child;
	PANIC(iter == NULL || iter->type != JSON_OBJECT);
	_layers = 1;
	
	while (iter != item->child_end)
	{
		_layers++;
		iter = iter->next;
	}
	
	// Allocating memory.
	_map.reserve(_width*_height*_layers);
	
	// Checking layer data.
	item = item->child; // First layer.
	
	for (unsigned z = 0 ; z < _layers ; z++)
	{
		iter = json_find_first_label(item,"data")->child;
		
		iter = iter->child; // First entry in data.
		unsigned x = 0, y = 0;
		for (unsigned i = 0 ; i < _width * _height ; i++)
		{
			x = i / _height;
			y = i % _height;
			
			setTile(x,y,z,atoi(iter->text));
			
			iter = iter->next;
		}
		
		item = item->next; // Next layer.
	}
	
	json_free_value(&root);
	return true;
	
	panic:
		json_free_value(&root);
		return false;
}

inline unsigned Level::index (unsigned x, unsigned y, unsigned z)
{
	return x + y * _width + z * _width * _height;
}

Tile Level::getTile (unsigned x, unsigned y, unsigned layer)
{
	return _map[index(x,y,layer)];
}

void Level::setTile (unsigned x, unsigned y, unsigned layer, Tile val)
{
	_map[index(x,y,layer)] = val;
}
