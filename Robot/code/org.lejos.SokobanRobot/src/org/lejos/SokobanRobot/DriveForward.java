package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;



public class DriveForward  implements Behavior {
    private boolean suppressed = false;
    NXTRegulatedMotor MotorL = Motor.A;
    NXTRegulatedMotor MotorR = Motor.C;
    LightSensor linelight_right = new LightSensor(SensorPort.S3);
    LightSensor linelight_left = new LightSensor(SensorPort.S4);

    public boolean takeControl() {
        return true;
    }

    public void suppress() {
        suppressed = true;
    }

    public void action() {
        /*System.out.print("L: ");
        System.out.print(linelight_left.readValue());
        System.out.print(" R: ");
        System.out.println(linelight_right.readValue());*/
        suppressed = false;
        MotorL.forward();
        MotorR.forward();
        MotorL.setSpeed(800);
        MotorR.setSpeed(800);
    }
}
