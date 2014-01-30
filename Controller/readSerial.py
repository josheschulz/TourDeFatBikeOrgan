import serial
import sys
import datetime

ser = serial.Serial('/dev/ttyUSB0', 115200, xonxoff=True)

print "Starting at: "+str(datetime.datetime.now())

while True:
   try:
      sys.stdout.write(str(datetime.datetime.now()));
      sys.stdout.write("|");
      sys.stdout.write(ser.readline())
   except KeyboardInterrupt:
      break;

print "Stopped by User @ "+str(datetime.datetime.now());
