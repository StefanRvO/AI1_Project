package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;


public class LightSensor_Access  {
    LightSensor linelight_right = new LightSensor(SensorPort.S3);
    LightSensor linelight_left = new LightSensor(SensorPort.S4);
    LightSensor cross_light = new LightSensor(SensorPort.S1);
    public int get_left()
    {
        return linelight_left.readValue() + 1;
    }
    public int get_right()
    {
        return linelight_right.readValue();
    }
    public int get_cross()
    {
        return cross_light.readValue();
    }
}
