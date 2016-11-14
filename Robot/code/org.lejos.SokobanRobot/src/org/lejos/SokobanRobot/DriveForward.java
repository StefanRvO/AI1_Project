package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;


public class DriveForward  implements Behavior {
    private static final int INFINITY = 0;
    protected boolean suppressed = false;
    protected static NXTRegulatedMotor MotorL = Motor.A;
    protected static NXTRegulatedMotor MotorR = Motor.C;
    protected static int goal_degrees = 0; //Which tacho count we want to drive to.
                          //Zero means infinity.
    protected int maxSpeed = (int)(MotorL.getMaxSpeed() * 0.7 );
    protected static DriveForward instance = null;
    protected DecisionMaker the_brain = DecisionMaker.getInstance();
    protected static int start_degrees = 0;
    public boolean takeControl() {
        return true;
    }

    public static DriveForward getInstance()
    {
        if(instance == null) instance = new DriveForward();
        return instance;
    }

    protected DriveForward()
    {
    }


    public void suppress() {
        suppressed = true;
    }
    public static void set_goal_degrees(int goal)
    {
        //System.out.println("Goal set");
        DriveForward.goal_degrees = goal;
        DriveForward.start_degrees = MotorL.getTachoCount();

    }

    public void action() {
        suppressed = false;
        MotorL.forward();
        MotorR.forward();
        MotorL.setSpeed(maxSpeed);
        MotorR.setSpeed(maxSpeed);
        //System.out.println(this.goal_degrees);
        if(test_degrees())
        {
            this.the_brain.inform_can_placed();
        }
        try{
            Thread.sleep(5);
        }
        catch(InterruptedException e){}
    }

    public Boolean test_degrees()
    {
        if(DriveForward.goal_degrees != INFINITY)
        {
            int degree_driven = MotorL.getTachoCount() - DriveForward.start_degrees;
            if(degree_driven >= DriveForward.goal_degrees)
            {
                return true;
            }
        }
        return false;
    }
}
