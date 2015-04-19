#include <SDL2/SDL_mixer.h>

#ifndef USB_WARRIOR_SOUND_H
#define USB_WARRIOR_SOUND_H

#define SOUNDPLAYER_MAX_SOUNDS      32
#define SOUNDPLAYER_MAX_CHANNELS    32
#define SOUNDPLAYER_DEFAULT_VOLUME  (MIX_MAX_VOLUME / 25)

class SoundPlayer {
public:
	SoundPlayer();
	~SoundPlayer();

	void playMusic(const char* file);

	int loadSound(const char* file);
	void playSound(int id);

private:
	Mix_Music* _music;
	Mix_Chunk* _sounds[SOUNDPLAYER_MAX_SOUNDS];
	int _last_sound;

	void _crash(const char* msg);
};

#endif // USB_WARRIOR_SOUND_H
