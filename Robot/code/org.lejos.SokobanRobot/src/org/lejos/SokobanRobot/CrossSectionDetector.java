package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;
import java.lang.Thread;
import java.lang.Object;


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

    private RunningAverage RA_5 = new RunningAverage(frequency / 200, cross_light.readValue()); //2.5 ms    //was 100
    private RunningAverage RA_25 = new RunningAverage(frequency / 40, cross_light.readValue()); //50 ms
    private RunningAverage RA_100 = new RunningAverage(frequency / 20, cross_light.readValue()); //100 ms   //was 10

    private int counter = 0;


//  Hvis der er brug for det, så lav flere RA, som vi kan udskrive værdier fra.

    public void run()
    {
        int i = 0;

        RA_5.fill_with_samples(cross_light.readValue());
        RA_25.fill_with_samples(cross_light.readValue());
        RA_100.fill_with_samples(cross_light.readValue());

        while(!stop)
        {
            try{
                Thread.sleep( (int)(1./frequency) * 1000 );
            }
            catch(InterruptedException e){}

            int cur_val = cross_light.readValue();

            RA_5.add_sample(cur_val);
            RA_25.add_sample(cur_val);
            RA_100.add_sample(cur_val);

            last_diff = RA_100.get_average() - RA_5.get_average();

            if(last_diff > 7 && this.counter == 0){
                cross_section = true;
            }

            if( this.counter > 0 ){
                this.counter--;
            }
        }
    }
    protected CrossSectionDetector()
    {
        cross_thread = new Thread(this);
        cross_thread.start();
    }
    public void set_suspend_crossdector( int _timer )
    {
        this.counter = _timer;
    }
    public double get_last_diff()
    {
        return last_diff;
    }
    public double get_avg_5()
    {
        return this.RA_5.get_average();
    }
    public double get_avg_25()
    {
        return this.RA_25.get_average();
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
