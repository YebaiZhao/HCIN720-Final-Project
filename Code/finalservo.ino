//#include <Servo.h>
#include <stdio.h>
#include <stdlib.h>
Servo myservo1;
Servo myservo2;
unsigned long lastTime = 0UL;
char x[256];
int days;
int pos1 = 90;    // variable to store the servo position
int pos2 = 90;    // variable to store the servo position
int change1 = D4;
int change2 = D5;
int i = 0;
int dataTotal = 0;
int averageData = 0;
int eachLight;

//For webTemp data
int webTemp;
int webTempNum = 0;
int webTempTotal = 0;
int webTempAvg = 0;
int TEMP_MIN = 30;
int TEMP_RANGE = 50;

//For motion data
int ifMotion;
int ifMotionNum = 0;
int ifMotionTotal = 0;
int ifMotionAvg = 0;
int MOTION_MIN = 0;
int MOTION_RANGE = 10;

//The parameter for original state
int ORIGIN_POS = 90;
//The parameter for the range of servo
double SERVO_RANGE = 80;
//The parameter for the range of each source
double LIGHTRANGE = 1000;

//need to finish
//motor for two things(other from online)
//the motion for the first stepmotor
// the motion for the second stepmotor

void lightHandler(const char *event, const char *data)
{
  i++;
//   Serial.print(i);
//   Serial.print(event);
//   Serial.print(", data: ");
  if (data){
    //the range of the light
    days = atoi(data);
    dataTotal = dataTotal + days;
    averageData = dataTotal/i;
    // Serial.print(data);
    // Serial.print(", averageData");
    // Serial.println(averageData);
  }
  else
    Serial.println("NULL");
}

void motionHandler(const char *event, const char *data)
{
    ifMotionNum++;
    // Serial.print(ifMotionNum);
    // Serial.print(event);
    // Serial.print(", motion data: ");
    if (data){
        //the range of the light
        ifMotion = atoi(data)*10 - MOTION_MIN;
        ifMotionTotal = ifMotionTotal + ifMotion;
        ifMotionAvg = ifMotionTotal/ifMotionNum;
        // Serial.print(ifMotion);
        // Serial.print(", averageMotionData");
        // Serial.println(ifMotionAvg);
    }
    else
        Serial.println("NULL");
}

int tempChange(String command) {
    /* Spark.functions always take a string as an argument and return an integer.
    Since we can pass a string, it means that we can give the program commands on how the function should be used.
    In this case, telling the function "on" will turn the LED on and telling it "off" will turn the LED off.
    Then, the function returns a value to us to let us know what happened.
    In this case, it will return 1 for the LEDs turning on, 0 for the LEDs turning off,
    and -1 if we received a totally bogus command that didn't do anything to the LEDs.
    */
    webTempNum++;
    Serial.print("temperature data from the web is coming: ");
    Serial.print(command);
    
    webTemp = atoi(command) - TEMP_MIN;
    
    webTempTotal = webTempTotal + webTemp;
    Serial.print(", The total is: ");
    Serial.print(webTempTotal);
    webTempAvg = webTempTotal/webTempNum;
    Serial.print(", The average is: ");
    Serial.println(webTempAvg);
    return 1;
}

void setup() {
    Serial.begin(9600);
    Particle.subscribe("light", lightHandler, "2f0038000547343339373536");
    Particle.subscribe("motion", motionHandler, "2f0038000547343339373536");
    pinMode(D7, OUTPUT);
    pinMode(change1, OUTPUT);
    pinMode(change2, OUTPUT);
    myservo1.attach(change1);
    myservo2.attach(change2);
    //Spark.function("led",ledToggle);
    Spark.function("temp",tempChange);
    //eachLight = posForEachValue(LIGHTRANGE);
}

void loop() {
    unsigned long now = millis();
    //Every 5 seconds publish uptime
    if (now-lastTime>10000UL) {
        
        lastTime = now;
        
        //For light Sensor
        // Serial.print("movelightFrom: ");
        // Serial.print(ORIGIN_POS);
        // Serial.print(", lightAverageData");
        // Serial.print(averageData);
        pos1 = ORIGIN_POS + averageData*SERVO_RANGE/LIGHTRANGE;
        myservo1.write(pos1);
        // Serial.print(", to: ");
        // Serial.println(pos1);
        
        delay(1000);
        //push down
        pos1 = ORIGIN_POS;
        myservo1.write(pos1);
        // Serial.print("back");
        // Serial.println(pos1);
        delay(1000);
        
        
        //for the web data
        Serial.print("TempMoveFrom: ");
        Serial.print(ORIGIN_POS);
        Serial.print(", tempAverageData");
        Serial.print(webTempAvg);
        pos1 = ORIGIN_POS - webTempAvg*SERVO_RANGE/TEMP_RANGE;
        myservo1.write(pos1);
        Serial.print(", to: ");
        Serial.println(pos1);
        
        delay(1000);
        //push down
        pos1 = ORIGIN_POS;
        myservo1.write(pos1);
        Serial.print("back");
        Serial.println(pos1);
        delay(1000);
        
        
        
        
        
        
        
        
        
        
        
        //for Motion data
        // Serial.print("moveMotionFrom: ");
        // Serial.print(ORIGIN_POS);
        // Serial.print(", MotionAverageData");
        // Serial.print(ifMotionAvg);
        pos2 = ORIGIN_POS - ifMotionAvg*SERVO_RANGE/MOTION_RANGE;
        myservo2.write(pos2);
        // Serial.print(", to: ");
        // Serial.println(pos2);
        
        delay(1000);
        //push down
        pos2 = ORIGIN_POS;
        myservo2.write(pos2);
        // Serial.print("back");
        // Serial.println(pos2);
        
        
        ifMotionNum = 0;
        ifMotionTotal = 0;
        webTempNum = 0;
        webTempTotal = 0;
        i = 0;
        dataTotal = 0;
    }
}

//出来1的value，得到多少pos
double posForEachValue(double maxValue){
    double posForEach = SERVO_RANGE/maxValue;
    return posForEach;
}
