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

#ifndef _GAME_OBJECT_H_
#define _GAME_OBJECT_H_


#include <cassert>
#include <string>

#include "math.h"
#include "utils.h"
#include "geometry_component.h"


class GameObject;

class Scene;

class SpriteComponent;
class SoundComponent;

class GameObject;

enum {
	PLAYER_CONTROLLER_COMPONENT_ID,
	NOCLIP_MOVE_COMPONENT_ID,
	MOVE_COMPONENT_ID,

	// Must be the last.
	COMPONENT_COUNT
};


enum {
	COMP_ENABLED = 0x01
};


class LogicComponent {
public:
	LogicComponent(GameObject* obj);
	
	virtual void update() = 0;

	GameObject* object() const { return _obj; }
	bool isEnabled() const { return _flags & COMP_ENABLED; }
	void setEnabled(bool enabled);

protected:
	GameObject* _obj;
	unsigned    _flags;
};


enum {
	OBJECT_DESTROYED = 0x01,
	OBJECT_ACTIVE    = 0x02
};

enum {
	CURR_UP = 0,
	PREV_UP = 1
};


class GameObject {
public:
	GameObject(Scene* scene, const char* name);

	inline       Scene*       scene() const { return _scene; }
	inline const std::string& name () const { return _name;  }

	inline       GeometryComponent& geom(unsigned updateIndex = CURR_UP)
	{ assert(updateIndex < 2u); return _geom[updateIndex]; }
	inline const GeometryComponent& geom(unsigned updateIndex = CURR_UP) const
	{ assert(updateIndex < 2u); return _geom[updateIndex]; }

	void computeBoxFromSprite(const Vec2& anchor = Vec2::Zero(), float scale=1.f);

	inline bool isDestroyed() const { return _flags & OBJECT_DESTROYED; }
	inline bool isEnabled()    const { return _flags & OBJECT_ACTIVE; }
	bool hasComponent(unsigned id) const;
	LogicComponent* getComponent(unsigned id) const;

	inline Vec2 posInterp(double interp) const
	{ return lerp<Vec2>(interp, _geom[PREV_UP].pos, _geom[CURR_UP].pos); }
	inline Boxf boxInterp(double interp) const {
		return Boxf(
			lerp<Vec2>(interp, _geom[PREV_UP].box.min(), _geom[CURR_UP].box.min()),
			lerp<Vec2>(interp, _geom[PREV_UP].box.max(), _geom[CURR_UP].box.max())
		);
	}
	inline Boxf worldBoxInterp(double interp) const {
		Boxf prev = worldBox(PREV_UP);
		Boxf curr = worldBox(CURR_UP);
		return Boxf(
			lerp<Vec2>(interp, prev.min(), curr.min()),
			lerp<Vec2>(interp, prev.max(), curr.max())
		);
	}

	Boxf worldBox(unsigned updateIndex = 0) const;

	void setEnabled(bool active);
	
	void _nextUpdate();
	void _registerLogic(unsigned id, LogicComponent* lcomp);
	inline void _setFlags(unsigned flags) { _flags = flags; }

public:
	SpriteComponent*   sprite;
	SoundComponent*    sound;

private:
	typedef std::vector<LogicComponent*> LogicMap;

private:
	Scene*             _scene;

	unsigned           _flags;
	std::string        _name;

	GeometryComponent  _geom[2];
	LogicMap           _logicMap;
};


#endif
