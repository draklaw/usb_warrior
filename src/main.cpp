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

#include "game.h"
#include "main_state.h"
#include "test_state.h"


int main(int argc, char** argv) {
	Game game(argc, argv);
	game.initialize();

//	MainState state(&game);
//	TestState state(&game);

	game.creditState.image = "assets/titre.png";
	game.creditState.titleScreen = true;
	return game.run(&game.creditState);
}
