package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;
import java.lang.Thread;

public class CrossSectionDetector  extends Thread implements Behavior {
    //Thread CD_tread = new Thread(new CrossDetectorThread()).start();
    private boolean suppressed = false;
    LightSensor cross_light = new LightSensor(SensorPort.S1);
    int last_light_val = cross_light.readValue();
    private boolean cross_section = false;
    private static CrossSectionDetector instance = null;
    private Thread cross_thread = null;
    private boolean stop = false;
    public void run()
    {
        int frequency = 20;
        while(!stop)
        {
            try
            {
                Thread.sleep((int)(1./frequency) * 1000);
            }
            catch(InterruptedException e)
            {

            }
            int cur_val = cross_light.readValue();
            int diff = cur_val - last_light_val;
            last_light_val = cur_val;
            //if(diff > 2)
            //    System.out.println(diff);
            if(diff > 5) cross_section = true;
        }
    }
    protected CrossSectionDetector()
    {
        cross_thread = new Thread(this);
        cross_thread.start();
    }

    public static CrossSectionDetector getInstance()
    {
        if(instance == null) instance = new CrossSectionDetector();
        return instance;
    }

    public boolean takeControl()
    {

        return false;
    }

    public void suppress() {
        suppressed = true;
    }
    public boolean noticed_cross_section()
    {
        return cross_section;
    }
    public void unset_cross_section()
    {
        cross_section = false;
    }
    public void action() {
        return;
    }
}
