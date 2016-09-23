package org.lejos.SokobanRobot;

import java.util.ArrayList;
import lejos.nxt.LCD;
import java.util.StringTokenizer;

public class SokobanBoard {

    int size_x;
    int size_y;
    SokobanField player_field;
    ArrayList<ArrayList<SokobanField> > board;

    public static int get_digits(int x)
    {
        int len = 1;
        while(true)
        {
            x = x/10;
            if(x <= 0) break;
            len++;
        }
        return len;
    }


    SokobanBoard(String board_str)
    {
        StringTokenizer tokenizer = new StringTokenizer(board_str, "\n");
        ArrayList<ArrayList<SokobanField> > board_tmp = new ArrayList<ArrayList<SokobanField> >();
        while(tokenizer.hasMoreTokens())
        {
            String token = tokenizer.nextToken();
            board_tmp.add(this.parse_row(token, board_tmp.size()));
        }
        this.size_y = board_tmp.size();
        this.size_x = board_tmp.get(0).size();
        System.out.println(this.size_y);
        System.out.println(this.size_x);
        this.board = new ArrayList<ArrayList<SokobanField> >(this.size_x);
        for(int i = 0; i < this.size_x; i++) this.board.add(new ArrayList<SokobanField>());
        for(int x = 0; x < this.size_x; x++)
        {
            ArrayList<SokobanField>collumn = this.board.get(x);
            for(int y = 0; y < this.size_y; y++)
            {
                collumn.add(board_tmp.get(y).get(x));
            }
        }
        this.populate_neighbours();
    }

    String get_board_str()
    {
        String board_str = new String();
        for(int y = 0; y < this.size_y; y++)
        {
            String row_str = new String();
            for(int x = 0; x < this.size_x; x++)
            {
                row_str += Box_Type.get_box_char(this.board.get(x).get(y).type);
            }
            board_str += row_str;
            if(y != this.size_y -1)
                board_str += "\n";
        }
        return board_str;
    }

    void populate_neighbours()
    {
        for(int x = 0; x < this.size_x; x++)
        {
            for(int y = 0; y < this.size_y; y++)
            {
                SokobanField up = null;
                SokobanField down = null;
                SokobanField left = null;
                SokobanField right = null;
                if(y > 0) up = this.board.get(x).get(y - 1);
                if(y + 1 < this.size_y) down = this.board.get(x).get(y + 1);
                if(x > 0) left = this.board.get(x - 1).get(y);
                if(x + 1 < this.size_x) right = this.board.get(x + 1).get(y);
                this.board.get(x).get(y).set_neighbours(up, down, left, right);
                //Set player box if this is the one.
                if(this.board.get(x).get(y).type == Box_Type.Player || this.board.get(x).get(y).type == Box_Type.Player_On_Goal)
                    this.player_field = this.board.get(x).get(y);
            }
        }

    }
    void perform_move(Direction dir)
    {
        //System.out.println(player_field.x_pos + " " + player_field.y_pos);
        //System.out.println(player_field.is_moveable(dir));
        //System.out.println(player_field.get_neighbour(dir).x_pos + " " + player_field.get_neighbour(dir).y_pos + " " + player_field.get_neighbour(dir).type);
        if(player_field.move(dir))
            player_field = player_field.get_neighbour(dir);
    }

    ArrayList<SokobanField> parse_row(String row_str, int y_pos)
    {   //Parse the row given in the string.
        //Return a ArrayList of SokobanFields
        //Returns an empty ArrayList on error.
        //Allocate the row
        ArrayList<SokobanField> fields = new ArrayList<SokobanField>();
        for(int i = 0; i < row_str.length(); i++)
        {
            char this_char = row_str.charAt(i);
            Box_Type type = Box_Type.parse_char(this_char);
            int x_pos = fields.size();
            fields.add(new SokobanField(type, x_pos, y_pos));
        }
        return fields;
    }

    public Boolean is_completed()
    {
        for(int y = 0; y < this.size_y; y++)
        {
            for(int x = 0; x < this.size_x; x++)
            {
                if(this.board.get(x).get(y).type == Box_Type.Box)
                    return false;
            }
        }
        return true;
    }

}
