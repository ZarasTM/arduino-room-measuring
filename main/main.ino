#include <Servo.h>
#include "Wheels.h"
#include "PinChangeInterrupt.h"

// Pins for rotations
#define ROT_PIN_LEFT A0 
#define ROT_PIN_RIGHT A1

// Pins for sonar (HC-SR04)
#define TRIG_R A2
#define ECHO_R A3

#define TRIG_F A4
#define ECHO_F A5

// Max and min values for event horizon
#define HORIZON 200

#define MIN_DIST_R 10
#define MAX_DIST_R 30

#define MIN_DIST_F 30
#define MAX_DIST_F 0

// Circut of weheels in milimeters
#define CIRC 2125

Wheels w;

// Speed manipulation variables
int speedL, speedR, speedAll;

// Current distance returned by sonar
int fDist, rDist;

// Variables for stailizing ride
volatile int lGap, rGap;

// Variables for measuring distance
int lRot, rRot; // Rotations per second

void setup() {
  w.attach(6,7,5,9,8,10);

  // Set speed
  speedAll = 200;
  speedL = speedAll;
  speedR = speedAll;

  // Set gaps
  restartGaps();
  restartRot();
  
  w.setSpeedRight(speedR);
  w.setSpeedLeft(speedL);
  w.stop();
  
  // Interrupt pins
  pinMode(ROT_PIN_LEFT, INPUT); 
  pinMode(ROT_PIN_RIGHT, INPUT); 

  // Sonar pins
  pinMode(TRIG_F, OUTPUT);    // TRIG starts sonar
  pinMode(ECHO_F, INPUT);     // ECHO gets returning signal
  
  pinMode(TRIG_R, OUTPUT);
  pinMode(ECHO_R, INPUT);

  Serial.begin(9600);

  attachPCINT(digitalPinToPCINT(ROT_PIN_LEFT), increment, CHANGE);
  attachPCINT(digitalPinToPCINT(ROT_PIN_RIGHT), increment, CHANGE);
  delay(5000);
}

void loop() {
  checkPath();
  /*
  Serial.print(lGap);
  Serial.print(" ");
  Serial.println(rGap);
  restartGaps();
  */
  delay(200);
}

void increment(){
 if( (PINC & (1 << PC0)) ) lGap++;
 if( (PINC & (1 << PC1)) ) rGap++;
}

void checkPath(){
  fDist = getDistF();
  rDist = getDistR();
  
  Serial.print("fDist: ");
  Serial.print(fDist);
  Serial.print("    rDist: ");
  Serial.println(rDist);

  if(fDist < MIN_DIST_F){
    w.stop();
  }else{
    w.forward();
  }

  // SetSpeedLeft should be dependant
  if(rDist < MIN_DIST_R){
    Serial.println("Changing L speed");
    w.setSpeedLeft(100);
  }else if(rDist > MAX_DIST_R){
    Serial.println("Changing R speed");
    w.setSpeedRight(100);
  }else{
    Serial.println("Restarting speed");
    w.setSpeedRight(speedR);
    w.setSpeedLeft(speedL);
  }
}

int getDistF() {
  unsigned long tot;      // time of travel
  unsigned int distance;
  
  digitalWrite(TRIG_F, HIGH);
  delay(10);
  digitalWrite(TRIG_F, LOW);
  tot = pulseIn(ECHO_F, HIGH);

  distance = tot/58;
  
  return distance;
}

int getDistR() {
  unsigned long tot;      // time of travel
  unsigned int distance;
  
  digitalWrite(TRIG_R, HIGH);
  delay(10);
  digitalWrite(TRIG_R, LOW);
  tot = pulseIn(ECHO_R, HIGH);

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
