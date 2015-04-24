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

#include "uw_math.h"
#include "utils.h"
#include "geometry_component.h"


class GameObject;

class Scene;

class SpriteComponent;
class PlayerControlerComponent;
class NoclipMoveComponent;
class MoveComponent;
class BotComponent;
class TriggerComponent;
class WallComponent;


enum {
	CURR_UP = 0,
	PREV_UP = 1
};


class GameObject {
public:
	enum {
		DESTROYED = 0x01,
		ENABLED   = 0x02
	};

public:
	GameObject(Scene* scene, const char* name);

	inline       Scene*       scene() const { return _scene; }
	inline const std::string& name () const { return _name;  }

	template < typename T >
	T* getComponent() const {
		// One of the only correct use of const_cast. Good only because we
		// know that the non-const verison of getComponent does not modify
		// this.
		return const_cast<GameObject*>(this)->_getComponent<T>();
	}

	template < typename T >
	T*& _getComponent();

	inline       GeometryComponent& geom(unsigned updateIndex = CURR_UP)
	{ assert(updateIndex < 2u); return _geom[updateIndex]; }
	inline const GeometryComponent& geom(unsigned updateIndex = CURR_UP) const
	{ assert(updateIndex < 2u); return _geom[updateIndex]; }

	// TODO: move to sprite component
	void computeBoxFromSprite(const Vec2& anchor = Vec2::Zero(), float scale=1.f);

	inline bool isDestroyed() const { return _flags & DESTROYED; }
	inline bool isEnabled()   const { return _flags & ENABLED; }

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
	inline void _setFlags(unsigned flags) { _flags = flags; }

public:
	SpriteComponent*          sprite;
	PlayerControlerComponent* playerControler;
	NoclipMoveComponent*      noclipMove;
	MoveComponent*            move;
	BotComponent*             bot;
	TriggerComponent*         trigger;
	WallComponent*            wall;

private:
	Scene*             _scene;

	unsigned           _flags;
	std::string        _name;

	GeometryComponent  _geom[2];
};


template <>
inline SpriteComponent*& GameObject::_getComponent<SpriteComponent>() {
	return sprite;
}

template <>
inline PlayerControlerComponent*& GameObject::_getComponent<PlayerControlerComponent>() {
	return playerControler;
}

template <>
inline NoclipMoveComponent*& GameObject::_getComponent<NoclipMoveComponent>() {
	return noclipMove;
}

template <>
inline MoveComponent*& GameObject::_getComponent<MoveComponent>() {
	return move;
}

template <>
inline BotComponent*& GameObject::_getComponent<BotComponent>() {
	return bot;
}

template <>
inline TriggerComponent*& GameObject::_getComponent<TriggerComponent>() {
	return trigger;
}

template <>
inline WallComponent*& GameObject::_getComponent<WallComponent>() {
	return wall;
}


#endif
