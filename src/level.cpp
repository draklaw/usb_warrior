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


#define PANIC(test) do {		            \
	if (test) {	                            \
		printf (#test ": %i\n", __LINE__);  \
		json_free_value(&root);             \
		return false;                       \
	}	                                    \
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

Tile Level::getTile (unsigned x, unsigned y, unsigned layer) const
{
	return _map[index(x,y,layer)];
}

void Level::setTile (unsigned x, unsigned y, unsigned layer, Tile val)
{
	_map[index(x,y,layer)] = val;
}


bool Level::tileCollision(Tile tile) const {
	return tile > 0 && unsigned(tile) < _collisionTileSet.size() && _collisionTileSet[tile];
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
			Vec2i( box.min().x()    / tileSize.x(),
				   box.min().y()    / tileSize.y()),
			Vec2i((box.max().x()-1) / tileSize.x() + 1,
				  (box.max().y()-1) / tileSize.y() + 1)
	);
	Boxi maxBox = Boxi(Vec2i::Zero(), Vec2i(_width - 1, _height - 1));
	return tileBox.intersection(maxBox);
}


Boxf Level::tileBox(unsigned x, unsigned y) const {
	const Vec2i& tileSize = _tileMap.tileSize();
	Vec2 pos = Vec2(x*tileSize.x(), y*tileSize.y());
	return Boxf(pos, pos + tileSize.template cast<float>());
}


bool Level::collide(unsigned layer, const Boxf& box, CollisionInfo* info) const {
	if(info) {
		info->flags = 0;
		info->penetration = Vec2(0, 0);
	}
	Boxi boundBox = tileBounds(box);
	for(int y = boundBox.min().y(); y < boundBox.max().y(); ++y) {
		for(int x = boundBox.min().x(); x < boundBox.max().x(); ++x) {
			if(!tileCollision(getTile(x, y, layer))) continue;
			if(!info) return true;

			Boxf tBox = tileBox(x, y);
			Boxf inter = box.intersection(tBox);

//			_scene->game()->log("box: ", box.min().transpose(), ", ", box.sizes().transpose());
//			_scene->game()->log("tBox: ", tBox.min().transpose(), ", ", tBox.sizes().transpose());
//			_scene->game()->log("inter: ", inter.min().transpose(), ", ", inter.sizes().transpose());

			assert(!inter.isEmpty());

			if(inter.sizes().x() < inter.sizes().y()) {
				if(box.center().x() < tBox.center().x()) {
					info->flags |= CollisionInfo::RIGHT;
					info->penetration.x() = std::min(-inter.sizes().x(), info->penetration.x());
				} else {
					info->flags |= CollisionInfo::LEFT;
					info->penetration.x() = std::max( inter.sizes().x(), info->penetration.x());
				}
			} else {
				if(box.center().y() < tBox.center().y()) {
					info->flags |= CollisionInfo::BOTTOM;
					info->penetration.y() = std::min(-inter.sizes().y(), info->penetration.y());
				} else {
					info->flags |= CollisionInfo::TOP;
					info->penetration.y() = std::max( inter.sizes().y(), info->penetration.y());
				}
			}
		}
	}
	if(info) {
		return info->flags;
	}
	return false;
}


inline unsigned Level::index (unsigned x, unsigned y, unsigned z) const
{
	return x + y * _width + z * _width * _height;
}
