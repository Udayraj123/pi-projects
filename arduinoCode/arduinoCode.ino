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

/*

Switching - Auto And Manual


MQTT Graph

*/

// Temp & Humidity sensor
Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
bool automode = true;
ll optVal = 1500;//light's opt val

ll adjustLightTime = 15;//interval in seconds
ll minIrigationInterval = 5;//duration of watering in seconds

bool autoLock = true;
ll soilDryNessTHR = 550; //below this watering will start
ll HumidityTHR = 66; //below this watering will start

ll weatherForecastTime = 0;
ll weatherForecastDuration = 1800;

ll pos = 0;    // variable to store the servo position
ll maxAngle = 135;
ll minAngle = 0;
ll stepAngle = 15;
const int numReadings = 9;
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

    Serial.print ("Open Roof ");
    Serial.println(maxAngle);
moveSmooth(maxAngle);
}

void close() {

    Serial.print ("Close Roof ");
    Serial.println(minAngle);
moveSmooth(minAngle);
}

void moveSmooth(ll bestAngle){
  if(bestAngle<=maxAngle && bestAngle >=minAngle){
    if(bestAngle>pos){
    for (pos = pos; pos <= bestAngle; pos += 5) {
        myservo.write(pos);
        delay(30);
      }
      }
      else{
      for (pos = pos; pos >= bestAngle; pos -= 5) {
        myservo.write(pos);
        delay(30);
      }
    }
  }
  else{
  Serial.print(bestAngle);
  Serial.println(": Invalid Angle given!");
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
  
  moveSmooth(maxAngle);
  
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
    Serial.print("Found best angle :");
    Serial.print(bestAngle);
    Serial.print(" with Light: ");
    Serial.println(lightReadings[bestIndex]);

  moveSmooth(bestAngle);
}

void openIrigation(){
  
    Serial.println("Open Irigation ");
    if(digitalRead(IrigationPIN) == HIGH){
        Serial.println("Irigation is already open!");
    }
    analogWrite(IrigationPIN,255);
}

void openSpray(){
  
    Serial.println("Open Spray");
    if(digitalRead(SprayPIN) == HIGH){
        Serial.println("Spray is already open!");
    }
    analogWrite(SprayPIN,255);
}


void closeIrigation(){
    Serial.println("Close Irigation");
    if(digitalRead(IrigationPIN) == LOW){
        Serial.println("Irigation is already closed!");
    }
    analogWrite(IrigationPIN,0);
}

void closeSpray(){
  
    Serial.println("Close Spray");
    if(digitalRead(SprayPIN) == LOW){
        Serial.println("Spray is already closed!");
    }
    analogWrite(SprayPIN,0);
}
void switchMode(){
      automode= !automode;
 Serial.print("**SWITCHING MODE** auto = ");
    Serial.println(automode);

}

void lockOn(){
  time = millis();
  if(time - weatherForecastTime > weatherForecastDuration){
    
    autoLock = true;
    weatherForecastTime = time;  
  }

}
void setAngle(){
char angle = Serial.read();
ll angleSet = stepAngle*( angle-'0');
Serial.print("Setting roof angle to : ");
Serial.println(angleSet);
  moveSmooth(angleSet); 
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
if(automode){
    if((time - light_time) > 1000*adjustLightTime){
        adjustLight();
        light_time = time;
    }

    if((time - soilDryNess_time) > 1000*minIrigationInterval){

  if(time - weatherForecastTime > weatherForecastDuration){
   Serial.println("Weather Lock Timed out. Resuming Normal Irigation routine"); 
    autoLock=false; // timeout
  }
      if(autoLock==false){
      if(soilDryNess > soilDryNessTHR){
          
            openIrigation();
        }
        else{
            closeIrigation();
        }
        soilDryNess_time = time;
    }
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
      
      Serial.println("Reading input - ");
       char readChar = Serial.read ();

       if(readChar=='m')switchMode();
       else if(readChar=='l')lockOn();
    }
}
    else{

    if(Serial.available()){
       char readChar = Serial.read ();
        switch(readChar){
            case 'o': openIrigation();break;
            case 'c': closeIrigation();break;
            case 'p': openSpray();break;
            case 'v': closeSpray();break;
            case 'i': open(); break;
            case 'x': close(); break;
            case 's': setAngle(); break;

            case 'a': switchMode(); break;
            default: Serial.println("Invalid input!"); break;
        }        
    }
}
    delay(1000);
}
