#
# Test CMake version
#
CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

# The name of the project. (EDIT THIS)
PROJECT(SokobanSolver)

# Used to resolve absolute path names
SET(ROOT ${CMAKE_CURRENT_SOURCE_DIR})


# Set executable output dir to build  dir
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


SET( CMAKE_CXX_FLAGS  "	${CMAKE_CXX_FLAGS}	\
            		-Wno-error=unused-function  \
                    -Wno-error=unused-variable  \
			-std=c++14 		\
			-Ofast			\
		 	-Wall 			\
			-Wextra 		\
			-Wshadow 		\
			-Wpointer-arith 	\
			-ffunction-sections 	\
			-fdata-sections		\
			-Wcast-qual		\
			-Werror			\
			-Wno-unused-variable 	\
			-pedantic		\
			-Wunused-variable	\
			-g			\
" )

add_subdirectory("SokobanSolver")
