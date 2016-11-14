package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;

public class DecisionMaker  implements Behavior {
    NXTRegulatedMotor MotorL = Motor.A;
    NXTRegulatedMotor MotorR = Motor.C;

    private boolean suppressed = false;
    private CrossSectionDetector cross_detector = CrossSectionDetector.getInstance();
    private Turn turner = Turn.getInstance();
    private int maxSpeed = (int)(MotorL.getMaxSpeed() * 0.7 );

    //String lol = "ffffffffffffffff";
    //String lol = "flflfrfrfrfrflfl";
    String lol = "flflflflflflflfl";

    int position = 0;

    public boolean takeControl()
    {
        //System.out.println(cross_detector.noticed_cross_section());
        if(cross_detector.noticed_cross_section() && turner.turning() == false)
            return true;
        return false;
    }

    public void suppress() {
        suppressed = true;
    }

    public void action() {
        try{
            System.out.println("Detected cross section!");
            System.out.println(this.lol.charAt(this.position));
        }
        catch(Exception e){ this.position = 0; }

        if (this.lol.charAt(this.position) == 'f'){
            position++;
            cross_detector.unset_cross_section();
        }
        else if(this.lol.charAt(this.position) == 'l'){
            position++;
            cross_detector.unset_cross_section();
            turner.doTurn(Direction.left, 1);
        }
        else if(this.lol.charAt(this.position) == 'r'){
            position++;
            cross_detector.unset_cross_section();
            turner.doTurn(Direction.right, 1);
        }
        suppressed = false;

        if( this.lol.charAt(this.position) != 'f' ){
            /*
            *   Drive forward at full speed for a short while, so that the robot
            *   ends on the middle of a cross section.
            *   2016-11-14 - Tested from multiple directions with sleep 80 and 100
            *                   This works well with fully charged battery-pack
            */

            MotorL.setSpeed( maxSpeed );
            MotorR.setSpeed( maxSpeed );

            try{
                Thread.sleep( 200 );
            }
            catch(InterruptedException e){}

            MotorL.stop( true );
            MotorR.stop( true );
            while(MotorL.getRotationSpeed() != 0 && MotorR.getRotationSpeed() != 0);
        }

        return;
    }
}
