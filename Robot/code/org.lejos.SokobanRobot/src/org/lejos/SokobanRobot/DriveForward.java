package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;

public class DriveForward  implements Behavior {
    private boolean suppressed = false;
    NXTRegulatedMotor MotorL = Motor.A;
    NXTRegulatedMotor MotorR = Motor.C;

    private int maxSpeed = (int)(MotorL.getMaxSpeed() * 0.7 );

    public boolean takeControl() {
        return true;
    }

    public void suppress() {
        suppressed = true;
    }

    public void action() {
        suppressed = false;
        MotorL.forward();
        MotorR.forward();
        MotorL.setSpeed(maxSpeed);
        MotorR.setSpeed(maxSpeed);

        try{
            Thread.sleep(5);
        }
        catch(InterruptedException e){}
    }
}
