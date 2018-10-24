#!/bin/sh

while true
do
echo 1 > /sys/class/leds/led0/brightness;
echo HIGH;
sleep 0.5;
echo 0 >  /sys/class/leds/led0/brightness;
echo LOW;
sleep 0.5;
done 
