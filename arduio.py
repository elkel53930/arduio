import serial
import os
import sys
import struct
import time

INPUT  = "IN"
OUTPUT = "OUT"

HIGH = "1"
LOW = "0"

class Nack(Exception):
    """
    NACK
    """
    pass

class Arduio:
    NACK = "NACK"
    def __init__(self, port="/dev/ttyUSB0",baud=9600,timeout_ms=2000):
        if timeout_ms < 0:
            self._ser = serial.Serial(port,baud)
        else:
            self._ser = serial.Serial(port,baud,timeout=timeout_ms/1000.0)
        if self._ser.is_open:
            self._ser.reset_input_buffer()
            self._ser.reset_output_buffer()
    
    def set(self,port,io):
        self._ser.write(("SET," + str(port) + "," + io + "\n").encode())
        r = self._ser.readline().decode().rstrip().split(',')
        if r[0] == self.NACK:
            raise Nack

    def write(self,port,hilo):
        self._ser.write(("WRITE," + str(port) + "," + str(hilo) + "\n").encode())
        r = self._ser.readline().decode().rstrip().split(',')
        if r[0] == self.NACK:
            raise Nack

    def read(self,port):
        self._ser.write(("READ," + str(port) + "\n").encode())
        r = self._ser.readline().decode().rstrip().split(',')
        if r[0] == self.NACK:
            raise Nack
        return int(r[1])
    
    def read_all(self):
        self._ser.write(("READA\n").encode())
        r = self._ser.readline().decode().rstrip().split(',')
        if r[0] == self.NACK:
            raise Nack
        return list(map(int,r[1:8]))
    
    def read_analog(self,port):
        self._ser.write(("AREAD," + str(port) + "\n").encode())
        r = self._ser.readline().decode().rstrip().split(',')
        if r[0] == self.NACK:
            raise Nack
        return int(r[1])

    def pwm(self,port,duty):
        self._ser.write(("PWM," + str(port) + "," + str(duty) + "\n").encode())
        r = self._ser.readline().decode().rstrip().split(',')
        if r[0] == self.NACK:
            raise Nack