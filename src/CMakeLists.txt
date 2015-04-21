##
##  Copyright (C) 2015 the authors
##
##  This file is part of usb_warrior.
##
##  usb_warrior is free software: you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation, either version 3 of the License, or
##  (at your option) any later version.
##
##  usb_warrior is distributed in the hope that it will be useful, but
##  WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
##  General Public License for more details.
##
##  You should have received a copy of the GNU General Public License
##  along with usb_warrior.  If not, see <http://www.gnu.org/licenses/>.
##

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(json)

find_package(Eigen3 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_image REQUIRED)

include_directories(
	"${EIGEN3_INCLUDE_DIR}"
	"${SDL2_INCLUDE_DIR}"
	"json"
)

add_executable(usb_warrior
	main.cpp
	game.cpp
	game_state.cpp
	main_state.cpp
	test_state.cpp
	credit_state.cpp

	input.cpp
	image_manager.cpp
	sound_player.cpp
	font_manager.cpp
	loader.cpp

	game_object.cpp
	sprite_component.cpp
	level.cpp
	scene.cpp
	actions.cpp

	components/player_controler_component.cpp
	components/noclip_move_component.cpp
	components/move_component.cpp
	components/trigger_component.cpp
	components/bot_component.cpp
	components/wall_component.cpp
)

target_link_libraries(usb_warrior
	${SDL2_LIBRARY}
	${SDL2IMAGE_LIBRARY}
	${SDLMIXER_LIBRARY}
	${SDL2_LIBRARY}
	json
)
