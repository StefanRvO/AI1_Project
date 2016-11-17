#include <iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "Sokoban_Board.hpp"
#include "Solver.hpp"
#include <exception>
#include "DeadLockDetector.hpp"
#include <csignal>
#include <sstream>

namespace po = boost::program_options;

Solver *the_solver = nullptr;

std::string trim(std::string& str)
{
    if(!str.size()) return str;
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last-first+1));
}

std::vector<std::string> get_boards(std::string filename)
{
    std::vector<std::string> boards;
    std::ifstream infile(filename);
    if(!infile.good())
    {
        throw po::error("Provided file does not exist!!");
    }
    std::stringstream boards_ss;;
    boards_ss << infile.rdbuf();
    std::string current_board = "";
    for (std::string line; std::getline(boards_ss, line); )
    {
        std::string trimmed = trim(line);
        if(trimmed.size() && trimmed[0] == '#')
        {
            while(line[line.size() - 1] != '#') line.pop_back();
            current_board += line + '\n';
        }
        else if(current_board.size())
        {
            current_board.pop_back();
            boards.push_back(current_board);
            current_board = "";
        }
    }

    return boards;
}

void solve_board(std::string &board_str, po::variables_map &vm)
{
    Sokoban_Board board(board_str);
    //Create the solver
    Solver SSolver(&board, vm.count("silent"));
    the_solver = &SSolver;
    //Solve
    if(vm.count("max_time"))
    {
        SSolver.solve(milliseconds(vm["max_time"].as<uint64_t>()));
    }
    else
        SSolver.solve();
    if(vm.count("print_solution"))
    {
        std::cout << SSolver.get_player_moves() << std::endl;
    }

    if(vm.count("statistics"))
    {
        if(vm.count("print_solution") || !vm.count("silent"))
        {
            std::cout << "Visited nodes, TTable size, Player moves, Box moves, Cost, Time(ms)" << std::endl;
        }
        std::cout << SSolver.get_visited_nodes() << ", " << SSolver.ttable_size() <<
        ", " << SSolver.get_player_moves().size() << ", " << SSolver.get_box_move_count() << ", " <<
        SSolver.get_solution_cost() << ", " << SSolver.get_solve_time().count() << std::endl;
    }
}

void signal_handler(int signal)
{
    if(signal == SIGUSR1)
        std::cout << std::endl << "Visited nodes " << the_solver->get_visited_nodes() << std::endl <<
        "TTable size " <<the_solver->ttable_size() << std::endl
        << "Max bucket size: " << the_solver->max_ttable_size()<< std::endl << std::endl;
}


int main(int argc, char **argv)
{
    std::signal(SIGUSR1, signal_handler);
    //Setup Command line options.
    po::options_description desc("Usage of Sokoban Solver");
    desc.add_options()
    ("board,b", po::value<std::string>(), "The board to solve.")
    ("alternative,a", "This will cause the solver to expect a string format in the SDU format. (don't work with the \"file\" option.)")
    ("file,f", po::value<std::string>(),
    "Solve a board, or a collection of boards given in a file. The format of the file needs to be the following: \
    A new map is the text between two lines not containing # as the first character (appart from spaces).")
    ("max_time,m",po::value<uint64_t>(), "Maximum time allowed to use for solving the board, in milliseconds (per level if file is given)")
    ("silent,s", "Make the solver silent, i.e., don't print current search depth etc.")
    ("print_solution,p", "Print the solution after the board have been solved")
    ("statistics", "Print statistics for the solved board")
    ("help,h", "Print help messages");
    po::variables_map vm;
    std::vector<std::string> boards;
    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc).run(), vm);
        if(1 != vm.count("file") + vm.count("board"))
        {
            std::cout << "Error:, one of either file or board is required, but not both!" <<  std::endl;
            std::cout << desc << std::endl;
            return 0;
        }
        if(vm.count("help"))
        {
            std::cout << "Tool for solving Sokoban puzzles." << std::endl;
            std::cout << desc << std::endl;
            return 0;
        }
        po::notify(vm);
        if(vm.count("board"))
        {   //Grab the board.
            std::string board_str = vm["board"].as<std::string>();
            boards.push_back(board_str);
        }
        if(vm.count("file"))
        {
            std::string boards_str = vm["file"].as<std::string>();
            boards = get_boards(boards_str);
        /*    for(auto &board_str : boards)
            {
                Sokoban_Board board(board_str);
                max_boxes = std::max(board.get_box_count(), max_boxes);
            }
            std::cout << max_boxes << std::endl;*/
        }
        if(vm.count("alternative"))
        {
            //Convert the board str.
            for(auto &board_str : boards)
            {
                boost::replace_all(board_str, "X", "#");
                boost::replace_all(board_str, ".", " ");
                boost::replace_all(board_str, "J", "$");
                boost::replace_all(board_str, "G", ".");
                boost::replace_all(board_str, "M", "@");
            }
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
        if(!vm.count("print_solution") && vm.count("statistics") && vm.count("silent") )
            std::cout << "Visited nodes, TTable size, Player moves, Box moves, Cost" << std::endl;
        for(auto &board_str : boards)
            solve_board(board_str, vm);
    }
    catch(std::exception &e)
    {
        std::cout << "ERROR: " << e.what() << std::endl << std::endl;
        return 2;
    }

    return 0;
}
