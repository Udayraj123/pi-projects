#!/usr/bin/python

import RPi.GPIO as GPIO
from time import sleep
import time 

GPIO.setmode(GPIO.BCM)

A,B,C,D,E = 5,6,13,19,26

GPIO.cleanup() #if they were in use before

GPIO.setup(E,GPIO.IN) #E
GPIO.setup(D,GPIO.IN) #D
GPIO.setup(C,GPIO.IN) #C
GPIO.setup(B,GPIO.OUT) #B
GPIO.setup(A,GPIO.OUT) #A

def oscillate():
        GPIO.output(A,GPIO.HIGH)
        GPIO.output(B,GPIO.HIGH)
        print("A = B = HIGH")
        sleep(0.5)
        GPIO.output(A,GPIO.LOW)
        GPIO.output(B,GPIO.LOW)
        print("A = B = LOW")
        sleep(0.5)

def oscillate2():
        GPIO.output(A,GPIO.HIGH)
        GPIO.output(B,GPIO.HIGH)
        print("A = B = HIGH")
        sleep(0.25)

        GPIO.output(A,GPIO.LOW)
        GPIO.output(B,GPIO.LOW)
        print("A = B = LOW")
        sleep(0.25)


timer= time.time()
while(True):
    if(GPIO.input(E) == GPIO.LOW):
        if(GPIO.input(C)==GPIO.LOW and GPIO.input(D)==GPIO.LOW):
                passed_t = time.time() - timer
                print("~E ~C ~D timer = %d" % (passed_t))
                if(passed_t >= 5):
                        print("END")
                        break
        else:
                print("E is low, others not all Low")
                pass # timer=time.time()
        
    elif(GPIO.input(C) == GPIO.LOW):
        print("Ehigh ~C T = 1s")
        timer = time.time()
        oscillate()
                
    elif(GPIO.input(D) == GPIO.HIGH):
        print("D T = 0.5s")
        timer  = time.time()
        oscillate2()
    else: 
        print("Invalid i")
        sleep(0.5)


GPIO.cleanup()
