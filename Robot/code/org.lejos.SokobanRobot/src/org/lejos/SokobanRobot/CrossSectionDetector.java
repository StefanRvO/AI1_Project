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
    public static final int frequency = 1000;
    private double last_diff = 0;
    private RunningAverage RA_5 = new RunningAverage(frequency / 200, cross_light.readValue()); //5 ms
    private RunningAverage RA_100 = new RunningAverage(frequency / 10, cross_light.readValue()); //100 ms


//  Hvis der er brug for det, så lav flere RA, som vi kan udskrive værdier fra.

    public void run()
    {
        int i = 0;
        while(!stop)
        {
            //if(i++ % 2000 == 0) System.out.println(i);
            try
            {
                Thread.sleep((int)(1./frequency) * 1000);
            }
            catch(InterruptedException e)
            {

            }
            int cur_val = cross_light.readValue();
            RA_5.add_sample(cur_val);
            RA_100.add_sample(cur_val);
            last_diff = RA_5.get_average() - RA_100.get_average();
            if(last_diff > 5 ) cross_section = true;
            //if(cur_val < 55 && last_light_val >= 55) cross_section = true;
            //last_light_val = cur_val;
        }
    }
    protected CrossSectionDetector()
    {
        cross_thread = new Thread(this);
        cross_thread.start();
    }
    public double get_last_diff()
    {
        return last_diff;
    }
    public double get_avg_5()
    {
        return this.RA_5.get_average();
    }
    public double get_avg_100()
    {
        return this.RA_100.get_average();

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
