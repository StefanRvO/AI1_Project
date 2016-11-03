#pragma once

#include <cstdint>
#include "Sokoban_Box.hpp"
#include <string>
#include <map>
#include <random>
#include <set>
class Sokoban_Board;
class Sokoban_Board
{
    private:
     std::uniform_int_distribution<int64_t> rand_gen;
     std::random_device rd;
     std::mt19937_64 gen;
    public:
        //The actual board structure.
        std::vector< std::vector <Sokoban_Box> > board;
        uint32_t size_x;
        uint32_t size_y;
        Sokoban_Box *upper_left_reachable = nullptr;
        std::set<Sokoban_Box *, Sokoban_Box> reachable_open_list;


        Sokoban_Box *player_box = nullptr; //pointer to the box with the player.
        std::map< Sokoban_Box *, Sokoban_Box *> board_boxes; //pointers to all boxes on the board.
                                        //Made as an hash map for fast access to elements to delete and add.
                                        //May be a bit weird, and could maybe be changed to another container.

        std::vector<Sokoban_Box *> goals; //Pointers to all goals.
                                          //Used when calculating heuristics.

        //Copy constructor. Create an identical board
        Sokoban_Board(Sokoban_Board &_board);

        //Construct the board from a string of the format
        //Given by <http://Sokobanno.de/wiki/index.php?title=Level_format>
        //The format is also explained below:

        //|  Level element  |  Character   |
        //|       Wall      |      #       |
        //|      Player     |      @       |
        //| Player on goal  |      +       |
        //|      Box        |      $       |
        //|   Box on goal   |      *       |
        //|      Goal       |      .       |
        //|     Floor       | (Space)/-/_  |

        //Example:

        //#####
        //#@$.#
        //#####

        //Newlines can be substituted with "|"
        //If the rest of the line is only walls,
        //they can be omitted.

        //Run length encoding is supported. E.g. "4#" instead of "####",
        //but limited to single digit. e.g. 12# is not allowed.
        Sokoban_Board(std::string &board_str);
        ~Sokoban_Board();
        //Return the board in the format explained above.
        std::string get_board_str(bool with_coords = false) const;
        void populate_neighbours();
        std::vector<move> find_possible_moves();

        void calc_reachable(Move_Direction last_move_dir);
        static void  find_possible_moves_rec(Move_Direction dir, Sokoban_Box *search_box,
            std::vector<Sokoban_Box *> &searched_fields, std::vector<move> &moves);

        void perform_move(move the_move, bool reverse = false, bool recalculate = true);
        int32_t get_heuristic();
        friend std::ostream& operator<<(std::ostream& os, const Sokoban_Board &_board)
        {
            os << _board.get_board_str();
            return os;
        }
        //Static functions
        static Box_Type parse_char(char chr);
        static char get_box_char(Box_Type type);
        static std::vector <Sokoban_Box> parse_row(const std::string &row_str, uint32_t y_pos);
        bool is_reachable(Sokoban_Box *box) const;
        void calc_reachable_helper(Sokoban_Box *neighbour, Sokoban_Box *current, float edge_cost, Move_Direction move_dir);
        float get_move_cost(move the_move); //Returns the move cost based on the reachable map.
        float get_turn_direction_cost(Move_Direction last_dir, Move_Direction this_dir);
        void calc_reachable_rec(Sokoban_Box *box);
};
