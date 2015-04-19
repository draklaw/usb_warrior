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
#include <cstring>
#include <cstdio>

#include <iostream>

#include "json/json.h"

#include "game.h"
#include "scene.h"
#include "level.h"


#define PANIC(test) do {               \
	if (test) {                          \
		printf (#test ": %i\n", __LINE__); \
		json_free_value(&root);            \
		return false;                      \
	}                                    \
} while (false)


Level::Level(Scene* scene)
    : _scene(scene) {
	_width = _height = _layers = 0;
}

void Level::setTileMap(const TileMap& tileMap) {
	_tileMap = tileMap;
}

// TODO: Convert to private method ? Add assertion ?
static inline bool isTileLayer (json_t* layer)
{
	return strcmp(json_find_first_label(layer,"type")->child->text,"tilelayer") == 0;
}

bool Level::loadFromJsonFile (const char* tiledMap)
{
	_scene->game()->log("Load level \"", tiledMap, "\"...");
	json_t* root = NULL;
	json_t* item = NULL;
	json_t* iter = NULL;
	
	// Parse data.
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
	unsigned objectLayers = 0;
	iter = item->child;
	_layers = 0;
	
	do {
		PANIC(iter == NULL || iter->type != JSON_OBJECT);
		if (isTileLayer(iter))
			_layers++;
		else
			objectLayers++;
		iter = iter->next;
	} while (iter != NULL);
	
	// Allocating memory.
	_map.reserve(_width*_height*_layers);
	
	// Checking map layer data.
	unsigned z = 0;
	item = item->child; // First layer.
	
	while (z < _layers)
	{
		PANIC(item == NULL);
		if (!isTileLayer(item))
		{
			item = item->next;
			continue;
		}
		
		iter = json_find_first_label(item,"data")->child;
		PANIC(iter == NULL || iter->type != JSON_ARRAY);
		
		iter = iter->child; // First entry in data.
		unsigned x = 0, y = 0;
		for (unsigned i = 0 ; i < _width * _height ; i++)
		{
			x = i % _width;
			y = i / _width;
			
			setTile(x, y, z, atoi(iter->text) - 1);
			
			iter = iter->next;
		}
		
		item = item->next; // Next layer.
		z++;
	}
	
	// Back to first layer (checking object layer data).
	item = item->parent->child;
	z = 0;
	
	while (z < objectLayers)
	{
		if (isTileLayer(item))
		{
			item = item->next;
			continue;
		}
		
		// First object.
		iter = json_find_first_label(item,"objects")->child->child;
		
		while (iter != NULL)
		{
			json_t* prop = NULL;
			EntityData e;
			
			e.emplace("name",json_find_first_label(iter,"name")->child->text);
			e.emplace("type",json_find_first_label(iter,"type")->child->text);
			e.emplace("x",json_find_first_label(iter,"x")->child->text);
			e.emplace("y",json_find_first_label(iter,"y")->child->text);
			
			// Pile up custom properties.
			prop = json_find_first_label(iter,"properties")->child->child;
			while (prop != NULL) {
				e.emplace(prop->text,prop->child->text);
				prop = prop->next;
			}
			
			_entities.push_back(e);
			
			// Next object.
			iter = iter->next;
		}
		
		item = item->next;
		z++;
	}
	
	json_free_value(&root);
	return true;
}

/*
void Level::dumpEntities ()
{
	for (auto& e:_entities)
	{
		for (auto& av:e)
			std::cout << av.first << ":" << av.second << "\t";
		std::cout << std::endl;
	}
}
*/

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
