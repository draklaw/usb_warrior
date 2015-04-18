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

#ifndef _GAME_H_

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>


class Game {
public:
	Game(int argc, char** argv);
	~Game();

	void init();
	void quit();

	int run();

	void sdlCrash(const char* msg);
	void imgCrash(const char* msg);

	template < typename... Args >
	void write(Args... args);

private:
	SDL_Window*    _window;
	SDL_Renderer*  _renderer;

};


#endif
