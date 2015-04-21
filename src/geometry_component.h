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

#ifndef _GEOMETRY_COMPONENT_H_
#define _GEOMETRY_COMPONENT_H_


#include "math.h"


class GeometryComponent {
public:
	GeometryComponent() : pos(0, 0), box(), rot(0) {}

	inline Boxf worldBox() const {
		return Boxf(pos + box.min(), pos + box.max());
	}

	inline Boxf ladderBox() const {
		Vec2 v = Vec2(4,0);
		return Boxf(pos + box.min() + v, pos + box.max() - v);
	}

	Vec2  pos;
	Boxf  box;
	float rot;
};


#endif
