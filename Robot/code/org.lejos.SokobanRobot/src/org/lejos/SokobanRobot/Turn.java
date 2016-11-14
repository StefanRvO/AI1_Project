package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;
import java.lang.Thread;
import lejos.robotics.navigation.DifferentialPilot;

public class Turn  extends Thread implements Behavior {
    //Thread CD_tread = new Thread(new CrossDetectorThread()).start();
    private static final int WAIT_TIME = 20; // wait time in milliseconds.
    private boolean suppressed = false;
    LightSensor linelight_right = new LightSensor(SensorPort.S3);
    LightSensor linelight_left = new LightSensor(SensorPort.S4);
    public int frequency = 1000;
    RunningAverage RA_R =  new RunningAverage(this.frequency / 400, linelight_right.readValue());
    RunningAverage RA_L =  new RunningAverage(this.frequency / 400, linelight_left.readValue());

    private CrossSectionDetector cross_detector = CrossSectionDetector.getInstance();
    NXTRegulatedMotor MotorL = Motor.A;
    NXTRegulatedMotor MotorR = Motor.C;
    //DifferentialPilot pilot = new DifferentialPilot(6., 16.2 + 1, MotorL, MotorR);
    public Direction direction = Direction.none;
    public int turn_count;
    private static Turn instance = null;
    private int maxSpeed = (int)(MotorL.getMaxSpeed() * 0.7 );

    public boolean takeControl()
    {
        if(turn_count > 0)
            return true;
        return false;
    }

    protected Turn(){
    }

    public static Turn getInstance()
    {
        if(instance == null) instance = new Turn();
        return instance;
    }

    public void suppress() {
        suppressed = true;
    }

    public void doTurn(Direction _direction, int _turn_count)
    {
        this.direction = _direction;
        this.turn_count = _turn_count;
    }

    public void action() {
        if(this.direction == Direction.left){
            System.out.println("Turning left");
            MotorL.backward();
            MotorR.forward();
        }
        else if(this.direction == Direction.right){
            System.out.println("Turning right");
            MotorL.forward();
            MotorR.backward();
        }
        MotorL.setSpeed( (int)(maxSpeed * 0.50)); //TODO: Make constant speed, should not depend on voltage.
        MotorR.setSpeed( (int)(maxSpeed * 0.50)); //TODO: Make constant speed, should not depend on voltage.
        //Wait a moment to make sure that we have turned of from the crossSection
        wait_for_new_crossection();

    }

    public void wait_for_new_crossection()
    {
        System.out.println("Waiting for CX");

        while(this.turn_count > 0)
        {
            try
            {
                /*
                *   2016-11-14
                *   The sleep time is tested,
                *   and the robot does turn far enough away from the black line
                */

                Thread.sleep(450); //Wait to turn away from line
                RA_R.fill_with_samples(linelight_right.readValue());
                RA_L.fill_with_samples(linelight_left.readValue());

                if(false){
                    MotorL.setSpeed(0);
                    MotorR.setSpeed(0);
                    while(true);
                }
            }
            catch(InterruptedException e) {}

            while(Math.abs(RA_R.get_average() - RA_L.get_average()) < 15)
            {
                try {
                    Thread.sleep((int)(1./this.frequency) * 1000);
                }
                catch(InterruptedException e){}

                RA_R.add_sample(linelight_right.readValue());
                RA_L.add_sample(linelight_left.readValue());
            }

            while(Math.abs(RA_R.get_average() - RA_L.get_average()) > 10)
            {
                try {
                    Thread.sleep((int)(1./this.frequency) * 1000);
                }
                catch(InterruptedException e){}

                RA_R.add_sample(linelight_right.readValue());
                RA_L.add_sample(linelight_left.readValue());
            }

            cross_detector.set_suspend_crossdector( 300 );

            if(this.turn_count > 0){
                this.turn_count--;
                cross_detector.unset_cross_section();
            }
        }

            MotorL.stop(true);
            MotorR.stop(true);
            while(MotorL.getRotationSpeed() != 0 && MotorR.getRotationSpeed() != 0);
            //Button.waitForAnyPress();
    }

    public Boolean turning(){
        return this.turn_count > 0;
    }

}
