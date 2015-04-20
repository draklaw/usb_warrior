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


#include <algorithm>

#include <SDL2/SDL_timer.h>

#include "game.h"

#include "game_state.h"


double secondsFromDuration(Duration d) {
	typedef std::chrono::duration<double> FDur;
	return std::chrono::duration_cast<FDur>(d).count();
}


Duration durationFromSeconds(double sec) {
	typedef std::chrono::duration<double> FDur;
	return std::chrono::duration_cast<Duration>(FDur(sec));
}


GameState::GameState(Game* game, const std::string& name, Duration updateTime)
    : _game(game),
      _name(name),
      _updateTime(updateTime),
      _frameTime(durationFromSeconds(1.01d / game->getRefreshRate())),
      _enabled(false),
      _running(false),
      _updateCount(0) {
}


void GameState::run() {
	if(!_enabled) {
		// Initialization
		initialize();
		_enabled = true;
	}

	_running = true;
	start();

	TimePoint time = now();
	_nextFrame  = time;
	_nextUpdate = time + _updateTime;

	while(_running) {
		_game->dispatchPendingEvents();

		time = now();
		TimePoint next_event = std::min(_nextUpdate, _nextFrame);

		if(next_event > time) {
			// Wait !
			SDL_Delay(std::max(unsigned(secondsFromDuration(next_event - time) * 1000), 1u));
		} else if(next_event == _nextUpdate) {
			// Update !
			update();
			++_updateCount;
			_nextUpdate += _updateTime;
		} else {
			// Frame ! (Everything is more funny with bangs !)
			double interp = 1.d
			        + (secondsFromDuration(time - _nextUpdate)
			          / secondsFromDuration(_updateTime));
			frame(interp);
			_nextFrame += _frameTime;
		}
	}

	stop();

	if(!_enabled) {
		shutdown();
	}
}


void GameState::pause() {
	_running = false;
}


void GameState::quit() {
	_enabled = false;
	_running = false;
}


TimePoint GameState::now() {
	return std::chrono::time_point_cast<Duration>(Clock::now());
}


void GameState::initialize() {
}

void GameState::shutdown() {
}

void GameState::start() {
}

void GameState::stop() {
}
