#include <cstdlib>
#include <iostream>

#include "soundplayer.h"


SoundPlayer::SoundPlayer() {
	music = NULL;
	for (int i = 0; i < SOUNDPLAYER_MAX_SOUNDS; i++)
		sounds[i] = NULL;
	last_loaded_sound = -1;

	if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024))
		crash("Failed to initialize SDL_Mixer");
	Mix_AllocateChannels(SOUNDPLAYER_MAX_CHANNELS);
}


SoundPlayer::~SoundPlayer() {
	if(music)
		Mix_FreeMusic(music);
	for (int i = 0; i < SOUNDPLAYER_MAX_SOUNDS; i++)
		 if (sounds[i])
			 Mix_FreeChunk(sounds[i]);
	Mix_CloseAudio();
}


void SoundPlayer::alert(const char* msg) {
	std::cerr << msg << ": " << Mix_GetError() << "\n";
}


void SoundPlayer::crash(const char* msg) {
	alert(msg);
	std::exit(EXIT_FAILURE);
}


void SoundPlayer::playMusic(const char* file) {
	if(music)
		Mix_FreeMusic(music);
	music = Mix_LoadMUS(file);
	if(!music)
		alert("Failed to load music file");
	else if(Mix_PlayMusic(music, -1))
		alert("Failed to play music");
}


int SoundPlayer::loadSound(const char* file) {
	last_loaded_sound = (last_loaded_sound + 1) % SOUNDPLAYER_MAX_SOUNDS;
	if(sounds[last_loaded_sound])
		Mix_FreeChunk(sounds[last_loaded_sound]);
	sounds[last_loaded_sound] = Mix_LoadWAV(file);
	if(!sounds[last_loaded_sound])
		alert("Failed to load sound file");
	return last_loaded_sound;
}


void SoundPlayer::playSound(int id) {
	if(!sounds[id])
		alert("No sound loaded");
	else if(Mix_PlayChannel(-1, sounds[id], 0))
		alert("Failed to play sound");
}
