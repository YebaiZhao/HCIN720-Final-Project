char publishString1[180];
char publishString2[180];
void setup()
{
  Serial.begin(9600);
  pinMode(A1, OUTPUT);
}

void loop()
{
    int lightSensor = analogRead(A2);
    int motionSensor = digitalRead(D2);

    sprintf(publishString1,"%d",lightSensor);
    Serial.println(publishString1);
    Particle.publish("light1991pwpw", publishString1);
    delay(1000);
    sprintf(publishString2,"%d",motionSensor);
    Serial.println(publishString2);
    Particle.publish("motion", publishString2);
    //Serial.println(analogRead(A1));
    delay(1000);
}
