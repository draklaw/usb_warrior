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

find_package(Eigen3 REQUIRED)
find_package(SDL2 REQUIRED)

include_directories(
	"${EIGEN3_INCLUDE_DIR}"
	"${SDL2_INCLUDE_DIR}"
)

add_executable(usb_warrior
	main.cpp
)

target_link_libraries(usb_warrior
	GLEW
	GL
	${SDL2_LIBRARY}
)
