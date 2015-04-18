#include <cstdlib>
#include <iostream>

#include <SDL2/SDL_mixer.h>


#include "soundplayer.h"


SoundPlayer::SoundPlayer() {
	_music = nullptr;
	for (int i = 0; i < SOUNDPLAYER_MAX_SOUNDS; i++)
		_sounds[i] = nullptr;
	_last_sound = -1;

	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024)) {
		_crash("Failed to initialize SDL_Mixer");
	}
	Mix_AllocateChannels(SOUNDPLAYER_MAX_CHANNELS);
	
	Mix_VolumeMusic(SOUNDPLAYER_DEFAULT_VOLUME);
}


SoundPlayer::~SoundPlayer() {
	if(_music) { Mix_FreeMusic(_music); }
	for (int i = 0; i < SOUNDPLAYER_MAX_SOUNDS; i++) {
		if (_sounds[i]) { Mix_FreeChunk(_sounds[i]); }
	}
	Mix_CloseAudio();
}


void SoundPlayer::_crash(const char* msg) {
	std::cerr << msg << ": " << Mix_GetError() << "\n";
	std::exit(EXIT_FAILURE);
}


void SoundPlayer::playMusic(const char* file) {
	if(_music) { Mix_FreeMusic(_music); }
	_music = Mix_LoadMUS(file);
	if(!_music) { _crash("Failed to load music file"); }
	else if(Mix_PlayMusic(_music, -1)) { _crash("Failed to play music"); }
}


int SoundPlayer::loadSound(const char* file) {
	_last_sound = (_last_sound + 1) % SOUNDPLAYER_MAX_SOUNDS;
	if(_sounds[_last_sound]) { Mix_FreeChunk(_sounds[_last_sound]); }
	_sounds[_last_sound] = Mix_LoadWAV(file);
	if(!_sounds[_last_sound]) { _crash("Failed to load sound file"); }
	return _last_sound;
}


void SoundPlayer::playSound(int id) {
	if(!_sounds[id]) { _crash("No sound loaded"); }
	else if(Mix_PlayChannel(-1, _sounds[id], 0)) { _crash("Failed to play sound"); }
}
