#include "application.h"
#include <stdio.h>
#include <stdlib.h>


/////////////////////////////////////////// The setting for Servo /////////////////////////////////////////////////
Servo myservo1;
Servo myservo2;
//unsigned long lastTime = 0UL;
char x[256];
int pos1 = 90;    // variable to store the servo position
int pos2 = 90;    // variable to store the servo position
int change1 = D0;
int change2 = D1;

//For lightdata
//int i = 0;
int lightData;
int lightDataNum = 0;
int lightTotal = 0;
int lightAve = 0;
int LIGHT_MIN = 0;
int LIGHT_RANGE = 1000;

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
int SERVO_LARGE = 80;
int SERVO_MIN = 10;
//The parameter for the range of each source
//4096 for a circle
int STEPFORCIRCLE = 4096;


/////////////////////////////////////////// The setting for motor /////////////////////////////////////////////////


///////////////////////////Set the gears number//////////////////////////////
//requirement: (HOUR_LGEARNUM/HOUR_SGEARNUM)*HOUR_DEGREE = 45*n
//number of gears in large wheel
int HOUR_LGEARNUM = 120;
//number of gears in small wheel
int HOUR_SGEARNUM = 16;
//number of seperation for every 30 degree
double HOUR_DEGREE = 6; //need to with 6 degree
//requirement: (MIN_LGEARNUM/MIN_SGEARNUM)*MIN_DEGREE = 45*n
int MIN_LGEARNUM = 45;
int MIN_SGEARNUM = 16;
double MIN_DEGREE = 6;


/////////////////////////////// Set the pin //////////////////////////////////
int HOUR_MOTOR1 = A0;
int HOUR_MOTOR2 = A1;
int HOUR_MOTOR3 = A2;
int HOUR_MOTOR4 = A3;
int MIN_MOTOR1 = D2;
int MIN_MOTOR2 = D3;
int MIN_MOTOR3 = D4;
int MIN_MOTOR4 = D5;


/////////// Change the time interval &&& Moving steps each time ///////////////

//12 minute, 120000*HOUR_DEGREE
unsigned long Hour_Time_Interval = 20000UL;
//5 minute, (related to MIN_DEGREE, 6 = 1min)
unsigned long Min_Time_Interval = 20000UL;
//步进电机转一圈8秒
// int StepForHour = -moveSteps(HOUR_DEGREE, HOUR_LGEARNUM, HOUR_SGEARNUM);
// int StepForMin = moveSteps(MIN_DEGREE, MIN_LGEARNUM, MIN_SGEARNUM);
//for testing
// int StepForHour = -4096;
// int StepForMin = 4096;
int StepForHour = (-1)*(STEPFORCIRCLE*HOUR_DEGREE*(HOUR_LGEARNUM/HOUR_SGEARNUM)/360);
int StepForMin = STEPFORCIRCLE*MIN_DEGREE*(MIN_LGEARNUM/MIN_SGEARNUM)/360;

int moveSteps(double LmoveDegree, int LGs, int SGs)
{
    //how many degree for small gears, for one move of big gear
    int moveSteps = STEPFORCIRCLE*LmoveDegree*(LGs/SGs)/360;
    //need to have factor of 3*3*5
    //how many steps needed for those degree
    // int moveSteps = STEPFORCIRCLE*SmoveDegree/360;
    //in order to make moveSteps to int, (l/s)(degreeEach), need to have factor of 3*3*5
    return moveSteps;
}

/////////////////////////////////////////////// Threads Setting //////////////////////////////////////////////////

////////////////////Thread parameter structure &&&&& Set the parameter/////////////////
typedef struct {
    int motor1;
    int motor2;
    int motor3;
    int motor4;
    int steps;
    unsigned long duration;
    unsigned long lastTime;
    unsigned long now;
} MOTOR_PARAM;

// Defines thread parameters
MOTOR_PARAM motor1Param = {HOUR_MOTOR1, HOUR_MOTOR2, HOUR_MOTOR3, HOUR_MOTOR4, StepForHour, Hour_Time_Interval, 0UL, 0UL};

MOTOR_PARAM motor2Param = {MIN_MOTOR1, MIN_MOTOR2, MIN_MOTOR3, MIN_MOTOR4, StepForMin, Min_Time_Interval, 0UL, 0UL};


/////////////////// Define the thread &&& Give Function of Thread ///////////////////
Thread* motor1Thread;
Thread* motor2Thread;

os_thread_return_t motor2MOVE(void* param){
    MOTOR_PARAM *p = (MOTOR_PARAM*)param;

    // Start never ending loop
    for(;;) {

        p->now = millis();
        //digitalWrite(p->pin, LOW);
        //Serial.print(p->now - p->lastTime);
        //Serial.print(p->lastTime);
        if ((p->now)-(p->lastTime)>(p->duration)) {
            Serial.print("run");
            p->lastTime = p->now;
            move(p->steps, p->motor1, p->motor2, p->motor3, p->motor4);
        }
    }
}

os_thread_return_t motor1MOVE(void* param){
    MOTOR_PARAM *p = (MOTOR_PARAM*)param;

    // Start never ending loop
    for(;;) {
        p->now = millis();
        //digitalWrite(p->pin, LOW);
        //Serial.print(p->now - p->lastTime);
        //Serial.print(p->lastTime);
        if ((p->now)-(p->lastTime)>(p->duration)) {
            Serial.print("run");

            // run step motor
            p->lastTime = p->now;
            move(p->steps, p->motor1, p->motor2, p->motor3, p->motor4);

            // run
            //For light Sensor
            Serial.print("movelightFrom: ");
            Serial.print(ORIGIN_POS);
            Serial.print(", lightAverageData");
            Serial.print(lightAve);
            pos1 = ORIGIN_POS + lightAve*(SERVO_LARGE-SERVO_MIN)/LIGHT_RANGE + SERVO_MIN;
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

            //for the web data
            // Serial.print("TempMoveFrom: ");
            // Serial.print(ORIGIN_POS);
            // Serial.print(", tempAverageData");
            // Serial.print(webTempAvg);
            pos1 = ORIGIN_POS - webTempAvg*(SERVO_LARGE-SERVO_MIN)/TEMP_RANGE - SERVO_MIN;
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
            //for Motion data
            // Serial.print("moveMotionFrom: ");
            // Serial.print(ORIGIN_POS);
            // Serial.print(", MotionAverageData");
            // Serial.print(ifMotionAvg);
            pos2 = ORIGIN_POS - ifMotionAvg*(SERVO_LARGE-SERVO_MIN)/MOTION_RANGE - SERVO_MIN;
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
            lightDataNum = 0;
            lightTotal = 0;
        }
    }
}




/////////////////////////////////////////////////// Begin to Run ///////////////////////////////////////////////////////
void setup() {

    Serial.begin(9600);
    Serial.println("Let's begin!");



    //Set pin mode
    pinMode(change1, OUTPUT);
    pinMode(change2, OUTPUT);
    pinMode(HOUR_MOTOR1, OUTPUT);
    pinMode(HOUR_MOTOR2, OUTPUT);
    pinMode(HOUR_MOTOR3, OUTPUT);
    pinMode(HOUR_MOTOR4, OUTPUT);
    pinMode(MIN_MOTOR1, OUTPUT);
    pinMode(MIN_MOTOR2, OUTPUT);
    pinMode(MIN_MOTOR3, OUTPUT);
    pinMode(MIN_MOTOR4, OUTPUT);

    //set the servo
    myservo1.attach(change1);
    myservo2.attach(change2);

    //set internet
    Particle.subscribe("light1991pwpw", lightHandler, "2f0038000547343339373536");
    Particle.subscribe("motion", motionHandler, "2f0038000547343339373536");
    Spark.function("temp",tempChange);

}

//setting for adjust
double hour_small_degree = 0.5;
int hour_large_degree = 6;
double minute_small_degree = 0.5;
int minute_large_degree = 6;
// int HOUR_LGEARNUM = 120;
// int HOUR_SGEARNUM = 16;
// int HOUR_DEGREE = 6;
// int MIN_LGEARNUM = 45;
// int MIN_SGEARNUM = 16;
// int MIN_DEGREE = 6;


void loop() {
    // Nothing here.
    //moveSteps(double LmoveDegree, int LGs, int SGs)
    if (Serial.available())
    {
       int state = int(Serial.read());
       //q -- HBL
       if(state == 113){
           int steps = moveSteps(hour_large_degree, HOUR_LGEARNUM, HOUR_SGEARNUM);
           move(steps, HOUR_MOTOR1, HOUR_MOTOR2, HOUR_MOTOR3, HOUR_MOTOR4);
       }
       //w -- HBR
       else if(state == 119){
           int steps = moveSteps(hour_large_degree*(-1), HOUR_LGEARNUM, HOUR_SGEARNUM);
           move(steps, HOUR_MOTOR1, HOUR_MOTOR2, HOUR_MOTOR3, HOUR_MOTOR4);
       }
       //e -- HSL
       else if(state == 101){
           int steps = moveSteps(hour_small_degree, HOUR_LGEARNUM, HOUR_SGEARNUM);
           move(steps, HOUR_MOTOR1, HOUR_MOTOR2, HOUR_MOTOR3, HOUR_MOTOR4);
       }
       //r -- HSR
       else if(state == 114){
           int steps = moveSteps(hour_small_degree*(-1), HOUR_LGEARNUM, HOUR_SGEARNUM);
           move(steps, HOUR_MOTOR1, HOUR_MOTOR2, HOUR_MOTOR3, HOUR_MOTOR4);
       }//a -- MBL
       else if(state == 97){
           int steps = moveSteps(minute_large_degree, MIN_LGEARNUM, MIN_SGEARNUM);
           move(steps, MIN_MOTOR1, MIN_MOTOR2, MIN_MOTOR3, MIN_MOTOR4);
       }
       //s -- MBR
       else if(state == 115){
           int steps = moveSteps(minute_large_degree*(-1), MIN_LGEARNUM, MIN_SGEARNUM);
           move(steps, MIN_MOTOR1, MIN_MOTOR2, MIN_MOTOR3, MIN_MOTOR4);
       }
       //d -- MSL
       else if(state == 100){
           int steps = moveSteps(minute_small_degree, MIN_LGEARNUM, MIN_SGEARNUM);
           move(steps, MIN_MOTOR1, MIN_MOTOR2, MIN_MOTOR3, MIN_MOTOR4);
       }
       //f -- MSR
       else if(state == 102){
           int steps = moveSteps(minute_small_degree*(-1), MIN_LGEARNUM, MIN_SGEARNUM);
           move(steps, MIN_MOTOR1, MIN_MOTOR2, MIN_MOTOR3, MIN_MOTOR4);
       }
       //g -- go
       else if(state == 103){
           Serial.println("let's go");
            //Start new threads
            motor1Thread = new Thread("sample", motor1MOVE, &motor1Param);
            motor2Thread = new Thread("sample", motor2MOVE, &motor2Param);
       }

       Serial.println(state);
    }
}

////////////////////////////////////////////////////// Helper Function ///////////////////////////////////////////////////////////





// void go(){
//     Serial.println("let's go");
//     //Start new threads
//     motor1Thread = new Thread("sample", motor1MOVE, &motor1Param);
//     motor2Thread = new Thread("sample", motor2MOVE, &motor2Param);

// }

// int moveSteps(int LmoveDegree, int LGs, int SGs)
// {
//     //how many degree for small gears, for one move of big gear
//     int SmoveDegree = LmoveDegree*(LGs/SGs);
//     //need to have factor of 3*3*5
//     //how many steps needed for those degree
//     int moveSteps = STEPFORCIRCLE*SmoveDegree/360;
//     //in order to make moveSteps to int, (l/s)(degreeEach), need to have factor of 3*3*5
//     return moveSteps;
// }

///////////////////////////////////// Motor Function //////////////////////////////////////////
int move(int steps, int MOTOR1, int MOTOR2, int MOTOR3, int MOTOR4){
  int eachStep = 0;
  if(steps > 0){
    //向上 up
    eachStep = -1;
  }
  if(steps < 0){
    //向下 down
    eachStep = 1;
  }
  int thisStep;
  int i = 0;
  int steps_left = abs(steps);

  while(i < abs(steps)){
    i = i + 1;
    steps_left = steps_left + eachStep;
    thisStep = ( steps_left % 8 ) + 1;
    switch (thisStep) {
      case 1:
      //case 100:
      digitalWrite(MOTOR1, HIGH);
      digitalWrite(MOTOR2, LOW);
      digitalWrite(MOTOR3, LOW);
      digitalWrite(MOTOR4, LOW);
      //Serial.print(500);
      break;
      case 2:
      //case 100*2:
      digitalWrite(MOTOR1, HIGH);
      digitalWrite(MOTOR2, HIGH);
      digitalWrite(MOTOR3, LOW);
      digitalWrite(MOTOR4, LOW);
      //Serial.print(1000);
      break;
      case 3:
      //case 100*3:
      digitalWrite(MOTOR1, LOW);
      digitalWrite(MOTOR2, HIGH);
      digitalWrite(MOTOR3, LOW);
      digitalWrite(MOTOR4, LOW);
      //Serial.print(1500);
      break;
      case 4:
      //case 100*4:
      digitalWrite(MOTOR1, LOW);
      digitalWrite(MOTOR2, HIGH);
      digitalWrite(MOTOR3, HIGH);
      digitalWrite(MOTOR4, LOW);
      //Serial.print(2000);
      break;
      case 5:
      //case 100*5:
      digitalWrite(MOTOR1, LOW);
      digitalWrite(MOTOR2, LOW);
      digitalWrite(MOTOR3, HIGH);
      digitalWrite(MOTOR4, LOW);
      //Serial.print(2500);
      break;
      case 6:
      //case 100*6:
      digitalWrite(MOTOR1, LOW);
      digitalWrite(MOTOR2, LOW);
      digitalWrite(MOTOR3, HIGH);
      digitalWrite(MOTOR4, HIGH);
      //Serial.print(3000);
      break;
      case 7:
      //case 100*7:
      digitalWrite(MOTOR1, LOW);
      digitalWrite(MOTOR2, LOW);
      digitalWrite(MOTOR3, LOW);
      digitalWrite(MOTOR4, HIGH);
      //Serial.print(3500);
      break;
      case 8:
      //case 100*8:
      digitalWrite(MOTOR1, HIGH);
      digitalWrite(MOTOR2, LOW);
      digitalWrite(MOTOR3, LOW);
      digitalWrite(MOTOR4, HIGH);
      //Serial.print(4000);
      break;
    }
    delay(2);
  }
  return 1;
}

///////////////////////////////////// Internet Function //////////////////////////////////////////
void lightHandler(const char *event, const char *data)
{
  lightDataNum++;
  Serial.print(lightDataNum);
  Serial.print(event);
  Serial.print(", data: ");
  Serial.print(data);
  if (data){
    //the range of the light
    lightData = atoi(data) - LIGHT_MIN;
    Serial.print(", after processing: ");
    Serial.print(lightData);
    lightTotal = lightTotal + lightData;
    lightAve = lightTotal/lightDataNum;

    Serial.print(", averageData");
    Serial.println(lightAve);
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

//clock adjustment function
