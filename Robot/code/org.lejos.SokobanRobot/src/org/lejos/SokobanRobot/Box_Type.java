package org.lejos.SokobanRobot;


public enum Box_Type
{
    Box,
    Goal_Box,
    Goal,
    Wall,
    Player,
    Player_On_Goal,
    Free;

    public static Box_Type parse_char(char chr)
    {
        switch(chr)
        {
            case '#': return Wall;
            case '*': return Goal_Box;
            case '.': return Goal;
            case '$': return Box;
            case '@': return Player;
            case '+': return Player_On_Goal;
            case ' ':
            case '-':
            case '_': return Free;
        }
        return Free;
    }
    public static char get_box_char(Box_Type type)
    {
        switch(type)
        {
            case Wall:            return '#';
            case Goal_Box:        return '*';
            case Goal:            return '.';
            case Box:             return '$';
            case Player:          return '@';
            case Player_On_Goal:  return '+';
            case Free:            return ' ';
        }
        return '-';
    }
}
