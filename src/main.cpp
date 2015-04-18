#include <cstdlib>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_events.h>


void sdlCrash(const char* msg) {
	std::cerr << msg << ": " << SDL_GetError() << "\n";
	std::exit(EXIT_FAILURE);
}


int main(int argc, char** argv) {
	unsigned initFlags = SDL_INIT_VIDEO
	                   | SDL_INIT_AUDIO
	                   | SDL_INIT_EVENTS;
	if(SDL_Init(initFlags)) {
		sdlCrash("Failed to initialize SDL");
	}

	unsigned windowFlags = 0;
//		| SDL_WINDOW_FULLSCREEN_DESKTOP
//		| SDL_WINDOW_OPENGL;
	SDL_Window* window = SDL_CreateWindow(
		"USB Warrior",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		800, 600,
		windowFlags);
	if(!window) { sdlCrash("Failed to create window"); }

	SDL_Event event;
	bool running = true;
	while(running) {
		SDL_WaitEvent(&event);

		std::cout << "Event: " << event.type << "\n";

		switch(event.type) {
			case SDL_QUIT: running = false; break;
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
