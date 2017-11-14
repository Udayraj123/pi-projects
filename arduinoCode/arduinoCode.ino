#include <Wire.h>
#include <Digital_Light_TSL2561.h>
#include "DHT.h"
#include <Servo.h>
/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#define ll long int
#define DHTPIN A0     // what pin we're connected to
#define MOISTUREPIN A8     
#define SERVOPIN 6     
#define IrigationPIN 11     
#define SprayPIN 12 // same since we have only one working relay

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Temp & Humidity sensor
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

ll threshold = 3000;
ll optVal = 15000;//light's opt val
ll adjustLightTime = 5;//interval in seconds
ll minIrigationInterval = 5;//duration of watering in seconds
ll soilDryNessTHR = 500; //below this watering will start
ll HumidityTHR = 500; //below this watering will start

ll pos = 0;    // variable to store the servo position
ll maxAngle = 120;
ll minAngle = 0;
ll stepAngle = 20;
const int numReadings = 12;
ll lightReadings[numReadings];

ll time = millis();
ll light_time = time;
ll soilDryNess_time = time;
ll Humidity_time = time;

DHT dht(DHTPIN, DHTTYPE);

void setup() 
{

    myservo.attach(SERVOPIN);  // attaches the servo on pin 9 to the servo object
    Serial.println("Initializing sensors : DHTxx test!");

    Serial.begin(9600); 
    // T & H
    dht.begin(); 
  // Light 
    Wire.begin(); 
    TSL2561.init(); 
    
}

void open() {
    if(pos>0)
        return;
    Serial.println("Opening roof\n");
    for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 90 degrees
    // in steps of 1 degree
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(30);                       // waits 15ms for the servo to reach the position
    }
}

void close() {
    if(pos==0)
        return;
    Serial.println("Closing roof\n");
    for (pos = maxAngle; pos > 0; pos -= 1) { // goes from 180 degrees to 0 degrees
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(30);                       // waits 15ms for the servo to reach the position
    }
}

void adjustLight(){
    //call this every light_time
    /*
    M1 - Sweep from maxAngle to minAngle
    record N_resolution values, choose the one closest
    move to that angle

    */
Serial.print("Finding best angle ");
ll index = 0,bestIndex,currRead,closestDiff=32000;
  
  for (pos = pos; pos <= maxAngle; pos += 10) {
    myservo.write(pos);
    delay(30);
  }
  
  for (pos = maxAngle; pos >= minAngle; pos -= stepAngle) { // goes from 180 degrees to 0 degrees
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(800);
        currRead =  TSL2561.readVisibleLux();
        lightReadings[index] = currRead;
        if(abs(currRead - optVal) <= closestDiff){
            closestDiff = abs(currRead - optVal);
            bestIndex = index;
        }
        index++;
    }
//adjust to best
    ll bestAngle = maxAngle - stepAngle*bestIndex;
    Serial.print("Adjusting roof to angle :");
    Serial.print(bestAngle);
    Serial.print(" with Light: ");
    Serial.println(lightReadings[bestIndex]);

    myservo.write(bestAngle);
}

void openIrigation(){
    if(digitalRead(IrigationPIN) == HIGH){
        Serial.println("Open: Irigation is already open!");
    }
    digitalWrite(IrigationPIN,HIGH);
}

void openSpray(){
    if(digitalRead(SprayPIN) == HIGH){
        Serial.println("Open: Spray is already open!");
    }
    digitalWrite(SprayPIN,HIGH);
}


void closeIrigation(){
    if(digitalRead(IrigationPIN) == LOW){
        Serial.println("Close : Irigation is already closed!");
    }
    digitalWrite(IrigationPIN,LOW);
}

void closeSpray(){
    if(digitalRead(SprayPIN) == LOW){
        Serial.println("Close: Spray is already closed!");
    }
    digitalWrite(SprayPIN,LOW);
}

void loop() 
{
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    ll light = TSL2561.readVisibleLux();
    ll soilDryNess = analogRead(MOISTUREPIN);

    time = millis();
    if((time - light_time) > 1000*adjustLightTime){
        adjustLight();
        light_time = time;
    }

    if((time - soilDryNess_time) > 1000*minIrigationInterval){
        if(soilDryNess > soilDryNessTHR){
            openIrigation();
        }
        else{
            closeIrigation();
        }
        soilDryNess_time = time;
    }
    
    if((time - Humidity_time) > 1000*minIrigationInterval){
        if(h < HumidityTHR){
            openSpray();
        }
        else{
            closeSpray();
        }
        Humidity_time = time;
    }
    
    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(t) || isnan(h) || light == -1) 
    {
        Serial.println("Invalid reading, Check Sensor Connection");
    } 
    else 
    {
       Serial.println("");
        Serial.print("sensorData: ");  // marker
        Serial.print("Humidity ");Serial.print(h); Serial.print(" ");
        Serial.print("Temperature "); Serial.print(t);Serial.print(" ");
        Serial.print("soilDryNess ");Serial.print(soilDryNess);Serial.print(" ");
        Serial.print("Light ");Serial.println(light);
        Serial.println("");
    }
    
    if(Serial.available()){
       char readChar = Serial.read ();
        switch(readChar){
            case 'o': openIrigation();break;
            case 'c': closeIrigation();break;
            case 'p': openSpray();break;
            case 'v': closeSpray();break;
            case 'i': open(); break;
            case 'x': close(); break;
            default:  break;
        }        
    }

    delay(1000);
}
