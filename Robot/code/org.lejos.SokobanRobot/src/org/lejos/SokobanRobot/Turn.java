package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;
import java.lang.Thread;
import lejos.robotics.navigation.DifferentialPilot;

public class Turn  extends Thread implements Behavior {
    //Thread CD_tread = new Thread(new CrossDetectorThread()).start();
    private boolean suppressed = false;
    LightSensor linelight_right = new LightSensor(SensorPort.S3);
    LightSensor linelight_left = new LightSensor(SensorPort.S4);
    NXTRegulatedMotor MotorL = Motor.A;
    NXTRegulatedMotor MotorR = Motor.C;
    DifferentialPilot pilot = new DifferentialPilot(6., 16.2 + 1, MotorL, MotorR);
    public Direction direction = Direction.none;
    public boolean takeControl()
    {
        return true;
        /*
        if(direction == Direction.none)
            return false;
        else
            return true;*/
    }

    public void suppress() {
        suppressed = true;
    }
    public void action() {
        direction = Direction.left;
        if(direction == Direction.left)
        pilot.rotate(90);
        else if(direction == Direction.right)
        pilot.rotate(-90);
        Button.waitForAnyPress();

    }
}
