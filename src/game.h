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
#include <memory>

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>

#include "uw_math.h"


class ImageManager;
class SoundPlayer;
class FontManager;

class GameState;
class MainState;
class CreditState;


class Game {
public:
	Game(int argc, char** argv);
	~Game();

	void initialize();
	void shutdown();

	void dispatchPendingEvents();

	void changeState(GameState* nextState);
	int run();
	void quit();

	Vec2i screenSize() const;
	bool  fullscreen() const;
	void  setFullscreen(bool enable);

	inline SDL_Renderer* renderer()  const { return _renderer; }
	inline ImageManager* images()          { return _imageManager.get(); }
	inline SoundPlayer*  sounds()          { return _soundPlayer.get(); }
	inline FontManager*  fonts()           { return _fontManager.get(); }

	inline MainState*    mainState()       { return _mainState.get(); }
	inline CreditState*  creditState()     { return _creditState.get(); }

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

private:
	SDL_Window*    _window;
	SDL_Renderer*  _renderer;

	std::unique_ptr<ImageManager>   _imageManager;
	std::unique_ptr<SoundPlayer>    _soundPlayer;
	std::unique_ptr<FontManager>    _fontManager;

	std::unique_ptr<MainState>      _mainState;
	std::unique_ptr<CreditState>    _creditState;

	GameState*     _state;
	GameState*     _nextState;
};


#endif
