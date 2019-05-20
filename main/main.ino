#include <Servo.h>
#include "Wheels.h"

// pins for sonar (HC-SR04)
#define TRIG A4
#define ECHO A5

// servo pin (has to be PWM)
#define SERVO 3

// Max and min values for event horizon
#define HORIZON 200
#define MIN_DIST 50
#define MAX_DIST 100

Servo serwo;

Wheels w;

int speedL, speedR, speedAll;
int dist;

void setup() {
  w.attach(9,8,10,6,7,5);
  
  speedAll = 255;
  speedL = speedAll;
  speedR = speedAll;

  w.setSpeed(speedAll);
  w.stop();

  pinMode(TRIG, OUTPUT);    // TRIG starts sonar
  pinMode(ECHO, INPUT);     // ECHO gets returning signal

  serwo.attach(SERVO);
  
  serwo.write(90);

  Serial.begin(9600);
}

void loop() {
  checkPath();
  delay(200);
}

void checkPath(){
  dist = lookAndTellDistance(90);
  
  Serial.print("Distance = ");
  Serial.println(dist);
  
  if(dist < MIN_DIST){
    w.back();
  }else if(dist > MAX_DIST && dist < HORIZON){
    w.forward();
  }else{
    w.stop();
  }
}

int lookAndTellDistance(byte angle) {
  unsigned long tot;      // time of travel
  unsigned int distance;

  serwo.write(angle);
  
  if(angle != 90){
    delay(500);
  }
  
  digitalWrite(TRIG, HIGH);
  delay(10);
  digitalWrite(TRIG, LOW);
  tot = pulseIn(ECHO, HIGH);

  distance = tot/58;
  
  return distance;
}
