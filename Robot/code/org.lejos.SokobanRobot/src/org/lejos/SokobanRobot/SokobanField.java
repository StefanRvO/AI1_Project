package org.lejos.SokobanRobot;



public class SokobanField
{
    SokobanField nb_up;
    SokobanField nb_down;
    SokobanField nb_left;
    SokobanField nb_right;
    int x_pos;
    int y_pos;
    Box_Type type;
    public SokobanField(Box_Type _type, int x_pos, int y_pos)
    {
        this.type = _type;
        this.x_pos = x_pos;
        this.y_pos = y_pos;
    }

    public void set_neighbours(SokobanField _nb_up, SokobanField _nb_down,
        SokobanField _nb_left, SokobanField _nb_right)
    {
        this.nb_up = _nb_up;
        this.nb_down = _nb_down;
        this.nb_left = _nb_left;
        this.nb_right = _nb_right;
    }
    public void change_type(Box_Type new_type)
    {
        this.type = new_type;
    }

    public SokobanField get_neighbour(Direction dir)
    {
        switch(dir)
        {
            case up: return this.nb_up;
            case down: return this.nb_down;
            case left: return this.nb_left;
            case right: return this.nb_right;
        }
        return this;
    }
    public Boolean is_moveable(Direction dir)
    {
        switch(this.get_neighbour(dir).type)
        {
            case Box:
            case Goal_Box:
            case Wall:
                return false;
        }
        return true;
    }
    public Boolean move(Direction dir)
    {
        Box_Type nb_type = this.get_neighbour(dir).type;

        if(nb_type == Box_Type.Box || nb_type == Box_Type.Goal_Box)
        {
            if(!this.get_neighbour(dir).is_moveable(dir)) return false;
                SokobanField.change_types_in_move(this.get_neighbour(dir), this.get_neighbour(dir).get_neighbour(dir));
        }
        else if(!this.is_moveable(dir)) return false;
        SokobanField.change_types_in_move(this, this.get_neighbour(dir));
        return true;
    }
    public static  void change_types_in_move(SokobanField old_box, SokobanField new_box)
    {
        switch(old_box.type)
        {
            case Box:
                old_box.change_type(Box_Type.Free);
                if(new_box.type == Box_Type.Goal) new_box.change_type(Box_Type.Goal_Box);
                else if(new_box.type == Box_Type.Free) new_box.change_type(Box_Type.Box);
                break;
            case Goal_Box:
                old_box.change_type(Box_Type.Goal);
                if(new_box.type == Box_Type.Goal) new_box.change_type(Box_Type.Goal_Box);
                else if(new_box.type == Box_Type.Free) new_box.change_type(Box_Type.Box);
                break;
            case Player:
                old_box.change_type(Box_Type.Free);
                if(new_box.type == Box_Type.Goal) new_box.change_type(Box_Type.Player_On_Goal);
                else if(new_box.type == Box_Type.Free) new_box.change_type(Box_Type.Player);
                break;
            case Player_On_Goal:
                old_box.change_type(Box_Type.Goal);
                if(new_box.type == Box_Type.Goal) new_box.change_type(Box_Type.Player_On_Goal);
                else if(new_box.type == Box_Type.Free) new_box.change_type(Box_Type.Player);
                break;
        }
    }
}
