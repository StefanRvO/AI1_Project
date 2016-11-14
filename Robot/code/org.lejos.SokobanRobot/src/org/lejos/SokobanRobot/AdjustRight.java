package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;



public class AdjustRight  implements Behavior {
    private int light_threshold = 10;
    private boolean suppressed = false;
    NXTRegulatedMotor MotorL = Motor.A;
    NXTRegulatedMotor MotorR = Motor.C;
    LightSensor linelight_right = new LightSensor(SensorPort.S3);
    LightSensor linelight_left = new LightSensor(SensorPort.S4);

    private int diff = linelight_left.readValue() - linelight_right.readValue();
    private int maxSpeed = (int)(MotorL.getMaxSpeed() * 0.7 );

    public boolean takeControl() {
        diff = linelight_left.readValue() - linelight_right.readValue();

        if( diff > light_threshold ) return true;
        return false;
    }

    public void suppress() {
        suppressed = true;
    }

    public void action() {
        suppressed = false;

        double thres = 0.6;

        if(diff < 15){
            double multiplier = (double)(light_threshold) / (diff);
            if(multiplier > thres) multiplier = thres;
            MotorL.setSpeed( (int)(maxSpeed * multiplier ) );
        }
        else if(diff < 25){
            double multiplier = (double)(light_threshold) / (diff * 1.6);
            if(multiplier > thres) multiplier = thres;
            MotorL.setSpeed( (int)(maxSpeed * multiplier ) );
        }
        else{
            double multiplier = (double)(light_threshold) / (diff * 2.1);
            if(multiplier > thres) multiplier = thres;
            MotorL.setSpeed( (int)(maxSpeed * multiplier ) );
        }
        MotorR.setSpeed( maxSpeed );

        MotorL.forward();
        MotorR.forward();

        try{
            Thread.sleep(5);
        }
        catch(InterruptedException e){}
    }
}
