import serial
import time

ser = serial.Serial('/dev/ttyUSB0', 57600, timeout=1)
ser.write('1'.encode('utf-8'))