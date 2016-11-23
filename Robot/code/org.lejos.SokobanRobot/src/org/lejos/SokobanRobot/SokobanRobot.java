package org.lejos.SokobanRobot;

import lejos.nxt.Button;
import lejos.nxt.LightSensor;
import lejos.nxt.SensorPort;
import lejos.robotics.subsumption.Arbitrator;
import lejos.robotics.subsumption.Behavior;
import java.lang.Thread;
import lejos.nxt.Sound;


public class SokobanRobot {

	public static void main(String[] args) {
        Sound.setVolume(20);
        LightSensor crosslight = new LightSensor(SensorPort.S1);
        LightSensor linelight_right = new LightSensor(SensorPort.S3);
        LightSensor linelight_left = new LightSensor(SensorPort.S4);
        crosslight.setFloodlight(true);
        linelight_right.setFloodlight(true);
        linelight_left.setFloodlight(true);
        System.out.println("TEST");
        Behavior b1_drive = DriveForward.getInstance();
        //if(b1_drive == null) System.out.print("1");
        Behavior ajust_left = AdjustLeft.getInstance();
        //if(ajust_left == null) System.out.print("2");
        Behavior ajust_right = AdjustRight.getInstance();
        //if(ajust_right == null) System.out.print("3");
        Behavior cross_detector = CrossSectionDetector.getInstance();
        //if(cross_detector == null) System.out.print("4");
        Behavior decision_maker = DecisionMaker.getInstance();
        //if(decision_maker == null) System.out.print("5");
        Behavior turner = Turn.getInstance();
        //if(turner == null) System.out.print("6");
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
