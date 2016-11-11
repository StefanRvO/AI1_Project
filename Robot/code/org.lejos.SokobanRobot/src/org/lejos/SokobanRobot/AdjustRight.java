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

    public boolean takeControl() {
        diff = linelight_left.readValue() - linelight_right.readValue();

        if(diff > light_threshold) return true;
        return false;
    }

    public void suppress() {
        suppressed = true;
    }

    public void action() {
        suppressed = false;
        MotorL.forward();
        MotorR.forward();

        MotorL.setSpeed( (int)MotorL.getMaxSpeed() );
        if(diff < 20)
        {
            double multiplier = (double)(light_threshold) / (diff);
            if(multiplier > 0.8) multiplier = 0.8;
            MotorR.setSpeed( (int)(MotorL.getMaxSpeed() * multiplier ) );
        }
        else if(diff < 30)
        {
            double multiplier = (double)(light_threshold) / (diff * 1.5);
            if(multiplier > 0.8) multiplier = 0.8;
            MotorR.setSpeed( (int)(MotorL.getMaxSpeed() * multiplier ) );
        }
        else
        {
            double multiplier = (double)(light_threshold) / (diff * 2);
            if(multiplier > 0.8) multiplier = 0.8;
            MotorR.setSpeed( (int)(MotorL.getMaxSpeed() * multiplier ) );
        }
        
        try{
            Thread.sleep(5);
        }
        catch(InterruptedException e){}
    }
}
