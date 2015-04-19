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

#include <cstdlib>
#include <iostream>

#include <SDL2/SDL_mixer.h>

#include "soundplayer.h"


#define SOUNDPLAYER_MAX_CHANNELS    32
#define SOUNDPLAYER_DEFAULT_VOLUME  (MIX_MAX_VOLUME / 25)


Sound::~Sound() {
	if(chunk) {
		Mix_FreeChunk(chunk);
	}
}


Music::~Music() {
	if(track) {
		Mix_FreeMusic(track);
	}
}


SoundPlayer::SoundPlayer(Game* game)
	: _game(game),
	  _music(nullptr),
	  _soundMap(),
	  _musicMap() {
	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024)) {
		_game->sndCrash("Failed to initialize SDL_Mixer");
	}
	Mix_AllocateChannels(SOUNDPLAYER_MAX_CHANNELS);
	Mix_VolumeMusic(SOUNDPLAYER_DEFAULT_VOLUME);
}


SoundPlayer::~SoundPlayer() {
	Mix_CloseAudio();
}


Sound SoundPlayer::loadSound(const std::string& filename) {
	auto it = _soundMap.find(filename);
	if(it == _soundMap.end()) {
		Sound snd;

		_game->log("Load sound \"", filename, "\"...");

		Mix_Chunk* chunk = Mix_LoadWAV(filename.c_str());
		if(!chunk) {
			_game->error("Failed to load sound: ", Mix_GetError());
			return Sound();
		}

		snd.chunk = chunk;
		snd.volume = SOUNDPLAYER_DEFAULT_VOLUME;
		Mix_VolumeChunk(snd.chunk, snd.volume);
		snd.name = filename;
		snd.useCount = 0;

		auto res = _soundMap.emplace(filename, snd);
		assert(res.second);
		it = res.first;

		// Do not destroy the chunk when snd goes out of scope
		snd.chunk = nullptr;
	}

	++(it->second.useCount);

	return it->second;
}


Music SoundPlayer::loadMusic(const std::string& filename) {
	auto it = _musicMap.find(filename);
	if(it == _musicMap.end()) {
		Music mus;

		_game->log("Load music \"", filename, "\"...");

		Mix_Music *track = Mix_LoadMUS(filename.c_str());
		if(!music.track) {
			_game->error("Failed to load music: ", Mix_GetError());
			return Music();
		}

		mus.track = track;
		mus.name = filename;
		mus.useCount = 0;

		auto res = _musicMap.emplace(filename, mus);
		assert(res.second);
		it = res.first;

		// Do not destroy the track when mus goes out of scope
		mus.track = nullptr;
	}

	++(it->second.useCount);

	return it->second;
}


bool SoundPlayer::playSound(Sound& sound) {
	return Mix_PlayChannel(-1, sound, 0)) == 0;
}


bool SoundPlayer::playMusic(Music& music) {
	if(_music) { Mix_FreeMusic(_music); }
	_music = &music;
	return _music->track && Mix_PlayMusic(_music->track, -1) == 0;
}
