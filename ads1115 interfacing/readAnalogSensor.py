#!/usr/bin/python

import RPi.GPIO as GPIO
from time import sleep
import time 

GPIO.setmode(GPIO.BCM)

A,B = 8,10

GPIO.cleanup() #if they were in use before
GPIO.setup(B,GPIO.IN) #B
GPIO.setup(A,GPIO.IN) #A


# Import the ADS1x15 module.
import Adafruit_ADS1x15



"""
Steps - 
Putty
setup bashrc (with sudo alias), 10proxy
sudo -E apt-get update
sudo -E apt-get install python-dev
sudo -E apt-get install git python-pip tightvnc 

sudo -E date -s "Oct 30 07:20 2017"
sudo -E pip install --upgrade pip
sudo -E pip install adafruit-ads1x15

mkdir ~/.config/autostart && sudo nano ~/.config/autostart/tightvnc.desktop
Add this contents - 
[Desktop Entry]
Type=Application
Name=TightVNC
Exec=vncserver :1
StartupNotify=false

Can Connect VNC now Optionally

sudo raspi-config
1. Expand Filesystem
2. Advanced-> I2C enable it

sudo apt-get install -y python-smbus i2c-tools

(many commands) Follow this link - https://learn.adafruit.com/adafruits-raspberry-pi-lesson-4-gpio-setup/configuring-i2c



R pi has specific SDA and SCL Pins
GPIO 2 & 3

Algo for simulation,
	take channel 0 readings per second.
	If the intensity is consistently high/low for n seconds,
	execute roof_open/roof_close accordingly

https://raspberrypi.stackexchange.com/questions/39313/do-all-gpio-pins-have-a-internal-pull-up-pull-down-resistors


Qs to ask -
> is adafruit code usable on other sensor

"""

# Create an ADS1115 ADC (16-bit) instance.
adc = Adafruit_ADS1x15.ADS1115()

# Or create an ADS1015 ADC (12-bit) instance.
#adc = Adafruit_ADS1x15.ADS1015()

# Note you can change the I2C address from its default (0x48), and/or the I2C
# bus by passing in these optional parameters:
#adc = Adafruit_ADS1x15.ADS1015(address=0x49, busnum=1)

# Choose a gain of 1 for reading voltages from 0 to 4.09V.
# Or pick a different gain to change the range of voltages that are read:
#  - 2/3 = +/-6.144V
#  -   1 = +/-4.096V
#  -   2 = +/-2.048V
#  -   4 = +/-1.024V
#  -   8 = +/-0.512V
#  -  16 = +/-0.256V
# See table 3 in the ADS1015/ADS1115 datasheet for more info on gain.
GAIN = 1


print('Reading ADS1x15 values, press Ctrl-C to quit...')
# Print nice channel column headers.
print('| {0:>6} | {1:>6} | {2:>6} | {3:>6} |'.format(*range(4)))
print('-' * 37)
# Main loop.
while True:
    # Read all the ADC channel values in a list.
    values = [0]*4
    for i in range(4):
        # Read the specified ADC channel using the previously set gain value.
        values[i] = adc.read_adc(i, gain=GAIN)
        # Note you can also pass in an optional data_rate parameter that controls
        # the ADC conversion time (in samples/second). Each chip has a different
        # set of allowed data rate values, see datasheet Table 9 config register
        # DR bit values.
        #values[i] = adc.read_adc(i, gain=GAIN, data_rate=128)
        # Each value will be a 12 or 16 bit signed integer value depending on the
        # ADC (ADS1015 = 12-bit, ADS1115 = 16-bit).
    # Print the ADC values.
    print('| {0:>6} | {1:>6} | {2:>6} | {3:>6} |'.format(*values))
    # Pause for half a second.
    time.sleep(0.5)


""" continous -

# Start continuous ADC conversions on channel 0 using the previously set gain
# value.  Note you can also pass an optional data_rate parameter, see the simpletest.py
# example and read_adc function for more infromation.
adc.start_adc(0, gain=GAIN)
# Once continuous ADC conversions are started you can call get_last_result() to
# retrieve the latest result, or stop_adc() to stop conversions.

# Note you can also call start_adc_difference() to take continuous differential
# readings.  See the read_adc_difference() function in differential.py for more
# information and parameter description.

# Read channel 0 for 5 seconds and print out its values.
print('Reading ADS1x15 channel 0 for 5 seconds...')
start = time.time()
while (time.time() - start) <= 5.0:
    # Read the last ADC conversion value and print it out.
    value = adc.get_last_result()
    # WARNING! If you try to read any other ADC channel during this continuous
    # conversion (like by calling read_adc again) it will disable the
    # continuous conversion!
    print('Channel 0: {0}'.format(value))
    # Sleep for half a second.
    time.sleep(0.5)

"""

# Stop continuous conversion.  After this point you can't get data from get_last_result!
adc.stop_adc()

GPIO.cleanup()
