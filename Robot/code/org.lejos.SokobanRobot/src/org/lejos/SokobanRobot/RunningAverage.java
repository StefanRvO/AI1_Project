package org.lejos.SokobanRobot;

import lejos.nxt.*;
import lejos.robotics.subsumption.*;
import lejos.nxt.Motor;
import lejos.robotics.subsumption.Behavior;



public class RunningAverage {

    public double average = 0;
    public int[] samples = null;
    public int size = 0;
    public int oldest_index = 0;
    public RunningAverage(int _size, int init_val)
    {
        this.samples = new int[_size];
        this.size = _size;
        this.average = init_val;
        for (int i = 0; i < _size; i++)
            samples[i] = init_val;
    }

    public void add_sample(int value)
    {
        int oldest_sample = this.samples[this.oldest_index];
        this.samples[this.oldest_index] = value;
        this.oldest_index = (this.oldest_index + 1) % this.size;

        double tmp = this.average * this.size;
        tmp -= oldest_sample;
        tmp += value;
        this.average = tmp / this.size;
    }
    public void fill_with_samples(int value)
    {
        this.average = value;
        for (int i = 0; i < this.size; i++)
            samples[i] = value;

    }
    public double get_average()
    {
        return this.average;
    }
}
