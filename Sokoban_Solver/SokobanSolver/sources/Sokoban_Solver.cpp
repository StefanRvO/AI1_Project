#include <iostream>
#include <boost/program_options.hpp>
#include "Sokoban_Board.hpp"
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
        for(auto &the_move : board.find_possible_moves())
        {
            std::cout << the_move.first << std::endl;
            std::cout << "(" << the_move.second->pos.x_pos << ",";
            std::cout << the_move.second->pos.y_pos << ")" << std::endl;
        }

        std::cout << board.get_board_str();
    }
    catch(std::exception &e)
    {
        std::cout << "ERROR: " << e.what() << std::endl << std::endl;
        return 2;
    }

    return 0;
}
