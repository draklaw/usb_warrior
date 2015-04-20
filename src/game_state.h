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

#ifndef _GAME_STATE_H_
#define _GAME_STATE_H_


#include <chrono>
#include <string>


typedef std::chrono::high_resolution_clock        Clock;
typedef std::chrono::nanoseconds                  Duration;
typedef std::chrono::time_point<Clock, Duration>  TimePoint;

class Game;


double   secondsFromDuration(Duration d);
Duration durationFromSeconds(double sec);


class GameState {
public:
	GameState(Game* game, const std::string& name, Duration updateTime);

	virtual void update() = 0;
	virtual void frame(double interp) = 0;

	inline bool isRunning() const { return _running; }
	inline const std::string& name() const { return _name; }

	void run();
	void pause();
	void quit();


	TimePoint now();

protected:
	virtual void initialize();
	virtual void shutdown();

	virtual void start();
	virtual void stop();

protected:
	Game*        _game;
	std::string  _name;

	Duration     _updateTime;
	Duration     _frameTime;
	unsigned     _maxUpdateLatency;

	bool         _enabled;
	bool         _running;

	TimePoint    _nextUpdate;
	TimePoint    _nextFrame;

	double       _uptime;
	unsigned     _updateCount;
};


#endif
