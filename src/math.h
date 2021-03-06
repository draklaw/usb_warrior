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

#ifndef _UW_MATH_H_
#define _UW_MATH_H_

#include <algorithm>
#include <ostream>

#include <Eigen/Dense>
#include <Eigen/Geometry>


typedef Eigen::Vector2f Vec2;
typedef Eigen::Vector2i Vec2i;

//typedef Eigen::Transform<float, 2> Transform;

typedef Eigen::AlignedBox2f Boxf;
typedef Eigen::AlignedBox2i Boxi;


inline std::ostream& operator<<(std::ostream& out, const Boxf& box) {
	out << "[ tl: " << box.min().transpose() << ", size: " << box.sizes().transpose() << "]";
	return out;
}


#endif
