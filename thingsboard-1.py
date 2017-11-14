import serial
from time import sleep
ser = serial.Serial('/dev/ttyS1',9600)

import paho.mqtt.client as mqtt
import time,json

def on_log(client, userdata, level, buf):
   print(buf) 
def on_connect(client, userdata, flags, rc):
    if rc==0:
        client.connected_flag=True #set flag
        print("connected OK")
    else:
        print("Bad connection Returned code=",rc)
        client.loop_stop()  
def on_disconnect(client, userdata, rc):
   print("client disconnected ok")
def on_publish(client, userdata, mid):
    print("In on_pub callback mid= "  ,mid)

mqtt.Client.connected_flag=False#create flag in class
mqtt.Client.suppress_puback_flag=False
client = mqtt.Client("python1")             #create new instance 
#client.on_log=on_log
client.on_connect = on_connect
client.on_disconnect = on_disconnect
client.on_publish = on_publish

broker="demo.thingsboard.io"
port =1883
topic="v1/devices/me/telemetry"

username="5a3QkqcfXpYw43FtniCn"
password=""

client.username_pw_set(username, password)
client.connect(broker,port)           #establish connection
while not client.connected_flag: #wait in loop
   client.loop()
   time.sleep(1)

time.sleep(3)
data=dict()
print("Init Done. Running main loop -")

for i in range(100):
    try: #serial can timeout
        sensorData = ser.readline()
        if("sensorData: " in sensorData):
            sensorData = sensorData.split(' ');
            print("sensorData: ",sensorData)
            for i,val in enumerate(sensorData):
                if(i>0 and i%2==1):#even are keys, odd are values, SKIP 1st element
                    data[val] = sensorData[i+1]
                    data_out=json.dumps(data) #create JSON object
                    print("publish topic",topic, "data out= ",data_out)
                    ret=client.publish(topic,data_out,0)    #publish
                    time.sleep(5)
                    client.loop()
        else:
            continue

    except:
        print("Error parsing data from Serial!")
        continue

client.disconnect()



