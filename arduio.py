import serial
import os
import sys
import struct
import time
import argparse

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
    def __init__(self, port="/dev/ttyUSB0",baud=115200,timeout_ms=3000):
        if timeout_ms < 0:
            self._ser = serial.Serial(port,baud)
        else:
            self._ser = serial.Serial(port,baud,timeout=timeout_ms/1000.0)
        if self._ser.is_open:
            self._ser.reset_input_buffer()
            self._ser.reset_output_buffer()
    
    def set(self,port,io):
        self._send("SET," + str(port) + "," + io + "\n")
        r = self._rcv()
        if r[0] == self.NACK:
            raise Nack

    def write(self,port,hilo):
        self._send("WRITE," + str(port) + "," + str(hilo) + "\n")
        r = self._rcv()
        if r[0] == self.NACK:
            raise Nack

    def read(self,port):
        self._send("READ," + str(port) + "\n")
        r = self._rcv()
        if r[0] == self.NACK:
            raise Nack
        return int(r[1])
    
    def read_all(self):
        self._send("READA\n")
        r = self._rcv()
        if r[0] == self.NACK:
            raise Nack
        return list(map(int,r[1:8]))
    
    def read_analog(self,port):
        self._send("AREAD," + str(port) + "\n")
        r = self._rcv()
        if r[0] == self.NACK:
            raise Nack
        return int(r[1])

    def pwm(self,port,duty):
        self._send("PWM," + str(port) + "," + str(duty) + "\n")
        r = self._rcv()
        if r[0] == self.NACK:
            raise Nack

    def _rcv(self):
        return self._ser.readline().decode().rstrip().split(',')

    def _send(self,str):
        return self._ser.write(str.encode())



if __name__ == "__main__":
    TABLE = {}
    TABLE["input"] = INPUT
    TABLE["output"] = OUTPUT

    parser =argparse.ArgumentParser(prog='arduio', description='arduio')

    parser.add_argument('-d', '--device', default="/dev/ttyUSB0", help='device file')
    parser.add_argument('command', help='set, write, read, read_all, read_analog')
    parser.add_argument('params', help='set PORT input/output, write PORT 0/1, read PORT, read_analog PORT, pwm PORT 0-255', nargs='*')
    args = parser.parse_args()

    a = Arduio(port=args.device)
    time.sleep(2)

    cmd = args.command
    params = args.params

    if cmd == "set":
        port = int(params[0])
        io = TABLE[params[1]]
        a.set(port,io)
    elif cmd == "write":
        port = int(params[0])
        a.write(port,params[1])
    elif cmd == "read":
        port = int(params[0])
        print(a.read(port))
    elif cmd == "read_all":
        print(a.read_all())
    elif cmd == "read_analog":
        port = int(params[0])
        print(a.read_analog(port))
    elif cmd == "pwm":
        port = int(params[0])
        duty = int(params[1])
        a.pwm(port,duty)
    else:
        print("invalid command")
        sys.exit(-1)
