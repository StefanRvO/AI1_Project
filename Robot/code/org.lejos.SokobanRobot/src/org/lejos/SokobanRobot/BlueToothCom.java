package org.lejos.SokobanRobot;

import java.io.*;
import lejos.nxt.*;
import lejos.nxt.comm.*;


/**
 * sample of selecting channel at run time
 */
public class BlueToothCom extends Thread
{
    NXTConnection connection = null;
    private boolean stop = false;
    private Thread com_thread = null;
    DataOutputStream dataOut = null;
    private CrossSectionDetector cross_detector = CrossSectionDetector.getInstance();
    LightSensor cross_light = new LightSensor(SensorPort.S1);
    LightSensor linelight_right = new LightSensor(SensorPort.S3);
    LightSensor linelight_left = new LightSensor(SensorPort.S4);
    NXTRegulatedMotor MotorL = Motor.A;
    NXTRegulatedMotor MotorR = Motor.C;


  public void run()
  {
      while(!stop)
      {
          try {
            StringBuilder sb = new StringBuilder();
            sb.append("\0RA5: ");
            sb.append(cross_detector.get_avg_5());
            sb.append("\tRA100: ");
            sb.append(cross_detector.get_avg_100());
            /*sb.append("\0");
            sb.append(MotorL.getMaxSpeed());
            sb.append(" ");
            sb.append(MotorR.getMaxSpeed());*/
            sb.append("\0");
            dataOut.writeUTF(sb.toString());
            Thread.sleep((int)(1./CrossSectionDetector.frequency) * 100);

          } catch (IOException e ) {
            System.out.println(" write error "+e);
        }
        catch(InterruptedException e)
        {

        }
      }
  }
  protected BlueToothCom()
  {

      System.out.println("Waiting for BT");
      connection = Bluetooth.waitForConnection();
      com_thread = new Thread(this);
      com_thread.start();
      dataOut = connection.openDataOutputStream();
  }

}
