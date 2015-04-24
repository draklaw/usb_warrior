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

#ifndef _UW_COMPONENT_H_
#define _UW_COMPONENT_H_


class Scene;
class GameObject;

class Component {
public:
	enum {
		ENABLED = 0x01
	};

public:
	Component(Scene* scene, GameObject* obj);
	virtual ~Component();

	inline Scene*      scene ()    const { return _scene; }
	inline GameObject* object()    const { return _obj; }

	inline bool        isEnabled() const { return _flags & ENABLED; }

	void setEnabled(bool enabled);

protected:
	Scene*      _scene;
	GameObject* _obj;
	unsigned    _flags;
};


#endif
