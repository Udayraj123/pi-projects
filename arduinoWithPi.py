import serial
from time import sleep
ser = serial.Serial('/dev/ttyS1',9600)
i = 0
while i<10:
	i+=1
	ser.write('3')
	sleep(1)
#	ser.readline()
