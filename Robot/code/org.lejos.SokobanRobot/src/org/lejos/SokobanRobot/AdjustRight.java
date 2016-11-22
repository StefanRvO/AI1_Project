package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;



public class AdjustRight  extends DriveForward implements Behavior {
    private int light_threshold = 10;
    LightSensor linelight_right = new LightSensor(SensorPort.S3);
    LightSensor linelight_left = new LightSensor(SensorPort.S4);
    private int diff = linelight_left.readValue() - linelight_right.readValue();
    protected static AdjustRight inherited_instance = null;

    public boolean takeControl() {
        diff = linelight_left.readValue() - linelight_right.readValue();

        if( diff > light_threshold ) return true;
        return false;
    }

    public static AdjustRight getInstance()
    {
        if(inherited_instance == null) inherited_instance = new AdjustRight();
        return inherited_instance;
    }

    protected AdjustRight()
    {
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
            MotorL.setSpeed( (int)(Settings.get_max_forward_speed() * multiplier ) );
        }
        else if(diff < 25){
            double multiplier = (double)(light_threshold) / (diff * 1.6);
            if(multiplier > thres) multiplier = thres;
            MotorL.setSpeed( (int)(Settings.get_max_forward_speed() * multiplier ) );
        }
        else{
            double multiplier = (double)(light_threshold) / (diff * 2.1);
            if(multiplier > thres) multiplier = thres;
            MotorL.setSpeed( (int)(Settings.get_max_forward_speed() * multiplier ) );
        }
        MotorR.setSpeed( Settings.get_max_forward_speed() );

        MotorL.forward();
        MotorR.forward();
        if(test_degrees())
        {
            this.the_brain.inform_can_placed();
        }

        try{
            Thread.sleep(5);
        }
        catch(InterruptedException e){}
    }
}
