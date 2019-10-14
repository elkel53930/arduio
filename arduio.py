import serial
import os
import sys
import struct
import time

class Arduio:
    def __init__(self, port="/dev/ttyUSB0",baud=9600,timeout_ms=500):
        if timeout_ms < 0:
            self._ser = serial.Serial(port,baud)
        else:
            self._ser = serial.Serial(port,baud,timeout=timeout_ms/1000.0)
        if self._ser.is_open():
            self._ser.reset_input_buffer()
            self._ser.reset_output_buffer()
    
    def set_port(self,port,io):

