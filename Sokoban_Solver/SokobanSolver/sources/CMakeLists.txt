#
# Test CMake version
#
CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)



# And now we add any targets that we want
add_executable	(Sokoban_Solver Sokoban_Solver.cpp Sokoban_Box.cpp Sokoban_Board.cpp
		Solver.cpp DeadLockDetector.cpp TransmutationTable.cpp TunnelMacroCreator.cpp
		Sokoban_Move.cpp
		)
target_link_libraries(Sokoban_Solver boost_program_options)
