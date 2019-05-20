#include <Servo.h>
#include "Wheels.h"
#include "PinChangeInterrupt.h"

// Pins for rotations
#define ROT_PIN_LEFT A0 
#define ROT_PIN_RIGHT A1

// Pins for sonar (HC-SR04)
#define TRIG A4
#define ECHO A5

// Servo pin (has to be PWM)
#define SERVO 3

// Max and min values for event horizon
#define HORIZON 200
#define MIN_DIST 50
#define MAX_DIST 100

// Circut of weheels in milimeters
#define CIRC 2125

Servo serwo;

Wheels w;

// Speed manipulation variables
int speedL, speedR, speedAll;

// Current distance returned by sonar
int dist;

// Variables for stailizing ride
volatile int lGap, rGap;

// Variables for measuring distance
int lRot, rRot; // Rotations per second

void setup() {
  w.attach(9,8,10,6,7,5);

  // Set speed
  speedAll = 255;
  speedL = speedAll;
  speedR = speedAll;

  // Set gaps
  restartGaps();
  restartRot();
  
  w.setSpeed(speedAll);
  w.stop();
  
  // Interrupt pins
  pinMode(ROT_PIN_LEFT, INPUT); 
  pinMode(ROT_PIN_RIGHT, INPUT); 

  // Sonar pins
  pinMode(TRIG, OUTPUT);    // TRIG starts sonar
  pinMode(ECHO, INPUT);     // ECHO gets returning signal

  serwo.attach(SERVO);
  
  serwo.write(90);

  Serial.begin(9600);

  attachPCINT(digitalPinToPCINT(ROT_PIN_LEFT), increment, CHANGE);
  attachPCINT(digitalPinToPCINT(ROT_PIN_RIGHT), increment, CHANGE);
}

void loop() {
  checkPath();
  Serial.print(rGap);
  Serial.print(" ");  
  Serial.println(lGap);
  delay(200);
}

void increment(){
 if( (PINC & (1 << PC0)) ) lGap++;
 if( (PINC & (1 << PC1)) ) rGap++;
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

void restartGaps(){
  rGap = 0;
  lGap = 0;
}

void restartRot(){
  rRot = 0;
  lRot = 0;
}
