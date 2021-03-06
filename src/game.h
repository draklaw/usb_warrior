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
#define _GAME_H_

#include <iostream>

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>


#include "math.h"
#include "image_manager.h"
#include "sound_player.h"
#include "font_manager.h"
#include "main_state.h"
#include "credit_state.h"


class GameState;
class SoundPlayer;


class Game {
public:
	Game(int argc, char** argv);
	~Game();

	void initialize();
	void shutdown();

	void dispatchPendingEvents();

	void changeState(GameState* nextState);
	int run(GameState* state);
	void quit();

	Vec2i screenSize() const;
	bool  fullscreen() const;
	void setFullscreen(bool enable);

	inline SDL_Renderer* renderer() const { return _renderer; }
	inline ImageManager* images() { return &_imageManager; }
	inline SoundPlayer* sounds() { return &_soundPlayer; }
	inline FontManager* fonts() { return &_fontManager; }

	int getRefreshRate() const;

	void sdlCrash(const char* msg);
	void imgCrash(const char* msg);
	void sndCrash(const char* msg);
	void fntCrash(const char* msg);

	template < typename... Args >
	void error(Args... args) {
		write("Error: ", args...);
	}

	template < typename... Args >
	void warning(Args... args) {
		write("Warning: ", args...);
	}

	template < typename... Args >
	void log(Args... args) {
		write(args...);
	}

	template < typename... Args >
	void lognr(Args... args) {
		writenr(args...);
	}

	template < typename T, typename... Args >
	inline void write(T first, Args... args) {
		std::cout << first;
		write(args...);
	}

	inline void write() {
		std::cout << "\n";
	}

	template < typename T, typename... Args >
	inline void writenr(T first, Args... args) {
		std::cout << first;
		writenr(args...);
	}

	inline void writenr() {
	}

public:
	MainState mainState;
	CreditState creditState;

private:
	SDL_Window*    _window;
	SDL_Renderer*  _renderer;

	ImageManager   _imageManager;
	SoundPlayer    _soundPlayer;
	FontManager    _fontManager;

	GameState*     _state;
	GameState*     _nextState;
};


#endif
