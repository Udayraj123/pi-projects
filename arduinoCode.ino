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

// Uncomment whatever type you're using!
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Temp & Humidity sensor
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

ll threshold = 3000;
ll pos = 0;    // variable to store the servo position
ll time = millis();
ll light_time = time;
ll soilMoisture_time = time;
DHT dht(DHTPIN, DHTTYPE);

void setup() 
{

    myservo.attach(6);  // attaches the servo on pin 9 to the servo object
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
    for (pos = 120; pos > 0; pos -= 1) { // goes from 180 degrees to 0 degrees
        myservo.write(pos);              // tell servo to go to position in variable 'pos'
        delay(30);                       // waits 15ms for the servo to reach the position
    }
}

void loop() 
{
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    ll light = TSL2561.readVisibleLux();
    ll soilMoisture = analogRead(MOISTUREPIN);
    time = millis();
    if((time - light_time) > 1000){
        if(light<threshold){
            open();
        }
        else{
            close();
        }
        light_time = time;
    }

    // if((time - soilMoisture_time) > 1000){
    //     if(soilMoisture<threshold){
    //         openValve();
    //     }
    //     else{
    //         closeValve();
    //     }
    //     soilMoisture_time = time;
    // }
    // check if returns are valid, if they are NaN (not a number) then something went wrong!
    if (isnan(t) || isnan(h) || light == -1) 
    {
        Serial.println("Check Sensor Connection");
    } 
    else 
    {
        Serial.print("Humidity: "); 
        Serial.print(h);
        Serial.print(" \t");
        Serial.print("Temperature: "); 
        Serial.print(t);
        Serial.print(" *C");
        Serial.print(" \t");
        Serial.print("Soil Moisture : "); 
        Serial.print(soilMoisture);
        Serial.print(" \t");
        Serial.print("The Light value is: ");
        Serial.println(light);
    }
    Serial.read();
}
