package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;

public class DecisionMaker  implements Behavior {
    private boolean suppressed = false;
    private CrossSectionDetector cross_detector = CrossSectionDetector.getInstance();
    NXTRegulatedMotor MotorL = Motor.A;
    NXTRegulatedMotor MotorR = Motor.C;
    public boolean takeControl()
    {
        //System.out.println(cross_detector.noticed_cross_section());
        if(cross_detector.noticed_cross_section())
            return true;
        return false;
    }
    public void suppress() {
        suppressed = true;
    }
    public void action() {
        suppressed = false;
        System.out.println("Detected cross section!");
        MotorL.setSpeed(0);
        MotorR.setSpeed(0);
        Button.waitForAnyPress();
        cross_detector.unset_cross_section();
        return;
    }
}
