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
    private int position = 0;
    private Boolean can_placed = false;
    //String lol = "flfrfrfrfrflflfbrfrflflflflfrfrb";  // 8-tal STORT
    //String lol = "flfrfrfrfrflflfl";  // 8-tal    <- awesome
    //String lol = "flflflfl"; // Kasse <- slightly less awesome
    //String lol = "flflflflbfrfrfrfrb"; // Kasse <- Med baglæns
    //String lol = "flffFbffrfFbflfffffFbfrfrffrffrfrfFbfrffrfflffflffFbflflfrffrfrffFbffrfrfFbflflfffffFbfrffrffrfrfFbffrffrfrfFbfrffrfflffflfFbflflflffffFbfflffrffrfFbfrfrfrfffFbflflflffFbffrflffrfrfFbfrfrfrffFbflflflfFbflfFbfffffrffrfffF";
    String lol = "fFbfrfrfFbfflfrflfflflfFbflffFbffrfFbflfffffFbfrfrffrffrfrfFbfrffrfflffflffFbflflfrffrfrffFbffrfrfFbflflfffffFbfrffrffrfrfFbffrffrfrfFbfrffrfflffflfFbflflflffffFbfflffrffrfFbfrfrfrfffFbflflflffFbffrflffrfrfFbfrfrfrffFbflflflfFbflfFbfffffrffrfffF"; // Kasse <- Med baglæns
    
    private static DecisionMaker instance = null;

    //String lol = "ffbrfrfrffb";

    protected DecisionMaker()
    {
    }

    public static DecisionMaker getInstance()
    {
        if(instance == null)
        {
            instance = new DecisionMaker();
        }
        return instance;
    }

    public boolean takeControl()
    {
        //System.out.println(cross_detector.noticed_cross_section());
        if( (cross_detector.noticed_cross_section() && turner.turning() == false) ||
            this.can_placed == true)
            return true;
        return false;
    }
    public void inform_can_placed()
    {
        this.can_placed = true;
    }

    public void suppress() {
        suppressed = true;
    }

    public void action() {
        try{
            System.out.println( this.lol.charAt(this.position) );
        }
        catch(Exception e){}
        if( this.position == this.lol.length() ){
            MotorL.stop( true );
            MotorR.stop( true );
            while(MotorL.getRotationSpeed() != 0 && MotorR.getRotationSpeed() != 0);
            System.exit(0);
        }
        this.can_placed = false;
        if (this.lol.charAt(this.position) == 'f'){
            cross_detector.set_suspend_crossdector( 10 );
        }
        else if (this.lol.charAt(this.position) == 'F'){
            cross_detector.set_suspend_crossdector( 10 );
            DriveForward.set_goal_degrees(425);
        }
        else if(this.lol.charAt(this.position) == 'l'){
            turner.doTurn(Direction.left, 1, false);
        }
        else if(this.lol.charAt(this.position) == 'L'){
            turner.doTurn(Direction.left, 1, true);
        }
        else if(this.lol.charAt(this.position) == 'r'){
            turner.doTurn(Direction.right, 1, false);
        }
        else if(this.lol.charAt(this.position) == 'R'){
            turner.doTurn(Direction.right, 1, true);
        }
        else if(this.lol.charAt(this.position) == 'b'){
            turner.doTurn(Direction.back, 1, false);
        }
        cross_detector.unset_cross_section();

        suppressed = false;
        if( this.lol.charAt(this.position) != 'f' && this.lol.charAt(this.position) != 'F' &&
            this.lol.charAt(this.position) != 'b')
        {
            /*
            *   Drive forward at full speed for a short while, so that the robot
            *   ends on the middle of a cross section.
            *   2016-11-14 - Tested from multiple directions with sleep 80 and 100
            *                   This works well with fully charged battery-pack
            */
            MotorL.setSpeed( Settings.get_max_forward_speed() );
            MotorR.setSpeed( Settings.get_max_forward_speed() );

            try{
                Thread.sleep( 150 );
            }
            catch(InterruptedException e){}

            //MotorL.stop( true );
            //MotorR.stop( true );
            //while(MotorL.getRotationSpeed() != 0 && MotorR.getRotationSpeed() != 0);
        }
        this.position++;
        return;
    }
}
