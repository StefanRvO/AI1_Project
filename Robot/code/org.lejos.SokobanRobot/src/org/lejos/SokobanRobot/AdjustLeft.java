package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;



public class AdjustLeft  implements Behavior {
    private int light_threshold = 5;
    private boolean suppressed = false;
    NXTRegulatedMotor MotorL = Motor.A;
    NXTRegulatedMotor MotorR = Motor.C;
    LightSensor linelight_right = new LightSensor(SensorPort.S3);
    LightSensor linelight_left = new LightSensor(SensorPort.S4);

    private int diff = linelight_right.readValue() - linelight_left.readValue();

    public boolean takeControl() {
        diff = linelight_right.readValue() - linelight_left.readValue();

        if( diff > light_threshold ){
            System.out.print( diff );
            System.out.print("\t");
            System.out.println( (int)(100*(1.3*light_threshold) / diff) );
            return true;
        }
        return false;
    }

    public void suppress() {
        suppressed = true;
    }

    public void action() {
        suppressed = false;
        MotorL.forward();
        MotorR.forward();

        //  The adjust speed needs to be inversely proportional.
        //      Mean that the difference is, the slower should the wheel turn.
        //      For ratio=1, maxSpeed * ratio would be forward.
        //      For ratio=0, maxSpeed * ratio, should mean the wheel doesn't turn at all.

        MotorL.setSpeed( (int)(MotorL.getMaxSpeed() * ( (1.2*light_threshold) / diff) ) );
        MotorR.setSpeed( (int)MotorL.getMaxSpeed() );
/*
        try{
            Thread.sleep(5);
        }
        catch(InterruptedException e){}
        */
    }
}
