#!/usr/bin/python2
#Utillity for connecting to an serial port which reponds to what is now called the "VSCProtocol" (Very Simple Communication Protocol)
#The utility logs the data to a folder, while, if choosen, also sending it to cout. This it done so another tool can grap it and make a live graph.
#The data in the folder is seperated in a file for each hour.


from serial import Serial
import sys
import os
import math
import time
import traceback

class CommandReciever():
    def __init__(self, tty, baud):
        self.serial = Serial()
        self.serial.port = tty
        self.serial.baudrate = baud	
        #self.serial.dsrdtr = False
        #self.serial.setDTR(False)
        self.serial.open()
        self.connected = True
        #time.sleep(1)

    def RecieveCommand(self):
        recieved = ""
        while True:
            out = self.serial.read(1)
            if(ord(out) >= 0x80): continue
            if out == "\0":
                if(len(recieved) >= 1):
                        break
                else:
                        recieved += " "
            recieved += out
        return recieved


def __main__():

    Commander = CommandReciever(sys.argv[1], int(sys.argv[2]))
    try:
            while(True):
                out_str = Commander.RecieveCommand()
                hex_str = ""
                for c in out_str:
                        hex_str += c.encode('hex') + ";"
                #print(hex_str)
                print(out_str)
    except:
            traceback.print_exc()
            Commander.serial.close()
            
    Commander.serial.close()
__main__()
