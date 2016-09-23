package org.lejos.SokobanRobot;

import lejos.nxt.Button;

/**
 * Example leJOS Project with an ant build file
 *
 */
public class SokobanRobot {

	public static void main(String[] args) {
		String board_str = "########\n## ##  #\n# $@$$ #\n#.#    #\n#.#$## #\n#   $. #\n#  .  .#\n########";
        SokobanBoard board = new SokobanBoard(board_str);
        while(true)
        {
            System.out.print("\n" + board.get_board_str());
            if(board.is_completed()) return;
            //for(int i = 0; i < 8; i++) System.out.println(board.board.get(i).get(3).type);
    		int button_press = Button.waitForAnyPress();
            Direction dir = Direction.up;
            switch(button_press)
            {
                case Button.ID_ENTER:
                    dir = Direction.up;
                    break;
                case Button.ID_ESCAPE:
                    dir = Direction.down;
                    break;
                case Button.ID_LEFT:
                    dir = Direction.left;
                    break;
                case Button.ID_RIGHT:
                    dir = Direction.right;
                    break;
            }
            if(Button.readButtons() == (Button.ID_ENTER | Button.ID_ESCAPE | Button.ID_LEFT | Button.ID_RIGHT))
                return;
            board.perform_move(dir);
        }
	}
}
