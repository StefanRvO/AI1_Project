package org.lejos.SokobanRobot;

import lejos.nxt.Button;
import lejos.nxt.LightSensor;
import lejos.nxt.SensorPort;
import lejos.robotics.subsumption.Arbitrator;
import lejos.robotics.subsumption.Behavior;
import java.lang.Thread;

public class SokobanRobot {

	public static void main(String[] args) {
        LightSensor crosslight = new LightSensor(SensorPort.S1);
        LightSensor linelight_right = new LightSensor(SensorPort.S3);
        LightSensor linelight_left = new LightSensor(SensorPort.S4);
        crosslight.setFloodlight(true);
        linelight_right.setFloodlight(true);
        linelight_left.setFloodlight(true);
        Behavior b1_drive = new DriveForward();
        Behavior ajust_left = new AdjustLeft();
        Behavior ajust_right = new AdjustRight();
        Behavior cross_detector = CrossSectionDetector.getInstance();
        Behavior decision_maker = new DecisionMaker();
        Behavior turner = Turn.getInstance();
        //BlueToothCom com = new BlueToothCom();
        Behavior [] b_array = {b1_drive, ajust_left, ajust_right, cross_detector, turner, decision_maker};
        Arbitrator arby = new Arbitrator(b_array);
        arby.start();
    /*    while (true)
        {
            crosslight.setFloodlight(true);
            linelight_right.setFloodlight(true);
            linelight_left.setFloodlight(true);
            System.out.println(crosslight.readValue());
            System.out.println(linelight_right.readValue());
            System.out.println(linelight_left.readValue());
            System.out.println();
            Button.waitForAnyPress();
            crosslight.setFloodlight(false);
            linelight_right.setFloodlight(false);
            linelight_left.setFloodlight(false);
            System.out.println(crosslight.readValue());
            System.out.println(linelight_right.readValue());
            System.out.println(linelight_left.readValue());
            System.out.println();

        }*/
	}

    void showSokobanBoard()
    {
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
