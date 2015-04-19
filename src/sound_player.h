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

#include <SDL2/SDL_mixer.h>

#ifndef _SOUND_PLAYER_H_
#define _SOUND_PLAYER_H_

#include <string>
#include <unordered_map>


#define SOUNDPLAYER_MAX_CHANNELS    32
#define SOUNDPLAYER_DEFAULT_VOLUME  (MIX_MAX_VOLUME / 25)


class Game;


class Sound {
public:
	~Sound();

	Mix_Chunk*   chunk;
	unsigned     volume;

	std::string  name;
	unsigned     useCount;
};


class Music {
public:
	~Music();

	Mix_Music*   track;

	std::string  name;
	unsigned     useCount;
};


class SoundPlayer {
public:
	SoundPlayer(Game *game);

	Sound loadSound(const std::string& filename);
	Music loadMusic(const std::string& filename);

	bool playSound(Sound& sound);
	bool playMusic(Music& music);

private:
	typedef std::unordered_map<std::string, Sound> SoundMap;
	typedef std::unordered_map<std::string, Music> MusicMap;

private:
	Game*    _game;

	SoundMap _soundMap;
	MusicMap _musicMap;	
};

#endif
