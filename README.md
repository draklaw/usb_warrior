# usb_warrior

Fight evil robots with your USB sticks !

Our modest participation to Ludum Dare 32. Coded from scratch with SDL2, Eigen and M's JSON parser.


## Compiling and running

The application has been tested successfully under Windows (with MinGW toolchain) and linux, and should work on MacOS X too.

Dependencies are:
- A C++ compiler (tested with GCC under Linux and MinGW under Windows)
- [cmake](http://www.cmake.org/)
- [Eigen3](http://eigen.tuxfamily.org/index.php)
- [SDL2](http://libsdl.org/) with [SDL2_image](http://www.libsdl.org/projects/SDL_image/) and [SDL2_mixer](http://www.libsdl.org/projects/SDL_mixer/).

On Debian-based Linux distributions (Debian, Ubuntu, Mint, ...), you can install all dependencies with (you may need to add `sudo` at the beginning)
```
apt-get install build-essential cmake libeigen3-dev libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev
```

To compile under *nux, run these:
```
cd <usb_warrior_dir>
mkdir build
cd build
cmake ..
make
```
Where `<usb_warrior_dir>` is the path to the directory of usb_warrior (the one that contains `src`). This will generate an executable `usb_warrior` in the `build/src` directory. **To run the game, the current directory should contain the assets directory**. Under *nux, you can type:
```
cd <usb_warrior_dir>
./build/src/usb_warrior
```
Running the game under Windows is a bit more involved because you need to either ensure that all dependencies are on the path, or to copy them in the same directory than the executable.
