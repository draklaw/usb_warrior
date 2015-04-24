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


#define PANIC(test) do {            			\
		if (test) {                         	\
			printf (#test ": %i\n", __LINE__);  \
			json_free_value(&root);             \
			return false;                       \
		}                                   	\
	} while (false)


int getInt(const EntityData& map, const char* key, int def) {
	auto it = map.find(key);
	return (it != map.end())? std::atoi(it->second.c_str()): def;
}


const std::string& getString(const EntityData& map, const char* key, const std::string& def) {
	auto it = map.find(key);
	return (it != map.end())? it->second: def;
}


void extractEntityMap(EntityData& map, json_t* node) {
	json_t* prop = node->child;
	while(prop) {
		switch(prop->child->type) {
		case JSON_STRING: {
			char* str = json_unescape(prop->child->text);
			map.emplace(prop->text, str);
			free(str);
			break;
		}
		case JSON_NUMBER:
			map.emplace(prop->text, prop->child->text);
			break;
		case JSON_TRUE:
			map.emplace(prop->text, "1");
			break;
		case JSON_FALSE:
		case JSON_NULL:
			map.emplace(prop->text, "0");
			break;
		case JSON_OBJECT:
			extractEntityMap(map, prop->child);
			break;
		case JSON_ARRAY:
			break;
		}
		prop = prop->next;
	}
}


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
	_entities.clear();
	_map.clear();
	_width = 0;
	_height = 0;
	_layers = 0;

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

		z++;
		if (item->next != NULL)
			item = item->next; // Next layer.
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
			EntityData e;
			extractEntityMap(e, iter);
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

Tile Level::getTile (unsigned x, unsigned y, unsigned layer) const
{
	return _map[index(x,y,layer)];
}

void Level::setTile (unsigned x, unsigned y, unsigned layer, Tile val)
{
	_map[index(x,y,layer)] = val;
}


bool Level::isClimbable (Tile t) const {
	return t == 771 || t == 772;
}


bool Level::tileCollision(Tile tile) const {
	return tile >= 0 && unsigned(tile) < _collisionTileSet.size() && _collisionTileSet[tile];
}


void Level::setTileCollision(Tile tile, bool collision) {
	assert(tile >= 0);
	if(_collisionTileSet.size() <= unsigned(tile)) {
		_collisionTileSet.resize(tile + 1, false);
	}
	_collisionTileSet[tile] = collision;
}


Boxi Level::tileBounds(const Boxf& box) const {
	const Vec2i& tileSize = _tileMap.tileSize();
	Boxi tileBox(
			Vec2i( box.min().x()  / tileSize.x(),
			       box.min().y()  / tileSize.y()),
			Vec2i((box.max().x()) / tileSize.x() + 1,
			      (box.max().y()) / tileSize.y() + 1)
	);
	Boxi maxBox = Boxi(Vec2i::Zero(), Vec2i(_width - 1, _height - 1));
	return tileBox.intersection(maxBox);
}


Boxf Level::tileBox(unsigned x, unsigned y) const {
	const Vec2i& tileSize = _tileMap.tileSize();
	Vec2 pos = Vec2(x*tileSize.x(), y*tileSize.y());
	return Boxf(pos, pos + tileSize.template cast<float>());
}


CollisionList Level::collide(unsigned layer, const Boxf& box, bool ladder) const {
	CollisionList inters;
	
	Boxi boundBox = tileBounds(box);
	for(int y = boundBox.min().y(); y < boundBox.max().y(); ++y) {
		for(int x = boundBox.min().x(); x < boundBox.max().x(); ++x) {
			if(   (!ladder && !tileCollision(getTile(x, y, layer)))
			   || (ladder && !isClimbable(getTile(x, y, layer))))
				continue;
			
			Boxf tBox = tileBox(x, y);
			inters.push_back(box.intersection(tBox));
		}
	}
	return inters;
}


inline unsigned Level::index (unsigned x, unsigned y, unsigned z) const
{
	return x + y * _width + z * _width * _height;
}
