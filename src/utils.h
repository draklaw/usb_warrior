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

#ifndef _UTILS_H_
#define _UTILS_H_


#define SDL_TRY(_code) do { if(_code) { _game->sdlCrash(__FILE__); } } while(false)

enum Direction {
	UP    = 0x01,
	RIGHT = 0x02,
	DOWN  = 0x04,
	LEFT  = 0x08
};

template < typename T >
T lerp(double f, const T& a, const T& b) {
	return (1. - f) * a + f * b;
}

#endif
