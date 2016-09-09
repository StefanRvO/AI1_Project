#include <iostream>
#include <boost/program_options.hpp>
#include "Sokoban_Board.hpp"
#include "Solver.hpp"
#include <exception>

namespace po = boost::program_options;


int main(int argc, char **argv)
{
    std::string board_str;
    //Setup Command line options.
    po::options_description desc("Usage of Sokoban Solver");
    desc.add_options()
    ("board,b", po::value<std::string>()->required(), "The board to solve.")
    ("help,h", "Print help messages");
    po::variables_map vm;
    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        if(vm.count("help"))
        {
            std::cout << "Tool for solving Sokoban puzzles." << std::endl;
            std::cout << desc << std::endl;
            return 0;
        }
        po::notify(vm);
        if(vm.count("board"))
        {   //Grab the board.
            board_str = vm["board"].as<std::string>();
        }
    }
    catch(po::error &e)
    {
            std::cout << "ERROR: " << e.what() << std::endl << std::endl;
            std::cout << desc << std::endl;
            return 1;
    }
    try
    {
        //Create the board.
        Sokoban_Board board(board_str);
        std::cout << board.get_board_str(true);

        //Create the solver
        Solver SSolver(&board);
        //Solve
        SSolver.solve();

        std::cout << board.get_board_str(true);
    }
    catch(std::exception &e)
    {
        std::cout << "ERROR: " << e.what() << std::endl << std::endl;
        return 2;
    }

    return 0;
}
