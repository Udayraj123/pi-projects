#!/usr/bin/python

import RPi.GPIO as GPIO
from time import sleep
import time 

GPIO.setmode(GPIO.BCM)

A,B,C,D,E = 6,6,13,19,26

GPIO.setup(E,GPIO.IN) #E
GPIO.setup(D,GPIO.IN) #D
GPIO.setup(C,GPIO.IN) #C
GPIO.setup(B,GPIO.OUT) #B
GPIO.setup(A,GPIO.OUT) #A

freq = 100
duty = 40

myWave = GPIO.PWM(A,freq)
myWave.start(duty)
timer= time.time()

while(True):

    if(GPIO.input(E) == GPIO.LOW):
        myWave.ChangeDutyCycle(40)

        print("E is LOW")
        pass
    else:
        print("E is HIGH")
        myWave.ChangeDutyCycle(80)
    

GPIO.cleanup()
