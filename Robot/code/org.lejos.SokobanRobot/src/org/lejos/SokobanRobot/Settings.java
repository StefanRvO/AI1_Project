package org.lejos.SokobanRobot;

import lejos.nxt.*;

import lejos.nxt.Motor;

public class Settings{
    static NXTRegulatedMotor MotorL = Motor.A;
    static NXTRegulatedMotor MotorR = Motor.C;

    static public int get_max_forward_speed()
    {
        return (int)(Settings.MotorL.getMaxSpeed() * 0.7 );
    }
    static public int get_max_turn_speed()
    {
        return (int)(Settings.MotorL.getMaxSpeed() * 0.7 );
    }
    static public int get_max_backward_speed()
    {
        return (int)(Settings.MotorL.getMaxSpeed() * 0.7 );
    }
}
