#include <SD.h>
#include "Wheels.h"
#include "PinChangeInterrupt.h"

// Pin for microSD card adapter
#define SD_PIN 4

// Pins for rotations
#define ROT_PIN_LEFT A0 
#define ROT_PIN_RIGHT A1

// Pins for sonar (HC-SR04)
#define TRIG_L A2
#define ECHO_L A3

#define TRIG_F A4
#define ECHO_F A5

// Max and min values for event horizon
#define HORIZON 200

#define MIN_DIST_L 10
#define MAX_DIST_L 20
#define MIN_TURN_DIST 100

#define MIN_DIST_F 30
#define MAX_DIST_F 0

// Circut of weheels in milimeters
#define CIRC 2125

File file;
Wheels w;

// Speed manipulation variables
int speedL, speedR, speedAll;

// Current distance returned by sonar
int fDist, lDist;

// Variables for stailizing ride
volatile int lGap, rGap;

void setup() {
  w.attach(6,7,5,9,8,10);

  // Set speed
  speedAll = 200;
  speedL = 200;
  speedR = 220;

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
  
  pinMode(TRIG_L, OUTPUT);
  pinMode(ECHO_L, INPUT);

  pinMode(SD_PIN, OUTPUT);
  if(!SD.begin(SD_PIN)){
    Serial.println("Could not initialize SD card.");
  }

  SD.remove("out.txt");

  Serial.begin(9600);
  
  attachPCINT(digitalPinToPCINT(ROT_PIN_LEFT), increment, CHANGE);
  attachPCINT(digitalPinToPCINT(ROT_PIN_RIGHT), increment, CHANGE);
  delay(1000);
}

void loop() {
  checkPath();
  delay(150);
}

void increment(){
 if( (PINC & (1 << PC0)) ) lGap++;
 if( (PINC & (1 << PC1)) ) rGap++;
}

void checkPath(){
  fDist = getDistF();
  lDist = getDistL();
  
  Serial.print("fDist: ");
  Serial.print(fDist);
  Serial.print("    lDist: ");
  Serial.println(lDist);

  if(fDist < MIN_DIST_F){
    Serial.println("Turning right");
    w.setSpeedRight(speedR);
    w.setSpeedLeft(speedL);
    w.stop();
    writeToAFile('R');
    w.forwardLeft();
    delay(800);
    w.stop();
  }else{
    w.forward();
  }
  
  int currSpeed;
  if(lDist < MIN_DIST_L){
    Serial.println("Changing R speed");
    w.setSpeedLeft(speedL);
    currSpeed = speedR - 100;
    w.setSpeedRight(currSpeed);
  }else if(lDist > MIN_TURN_DIST){
    Serial.println("Turning left");
    w.setSpeedRight(speedR);
    w.setSpeedLeft(speedL);
    w.stop();
    writeToAFile('L');
    w.forwardRight();
    delay(1000);
    w.stop();
    w.forward();
    delay(500);
  }else if(lDist > MAX_DIST_L){
    Serial.println("Changing L speed");
    w.setSpeedRight(240);
    currSpeed = speedL - 130;
    w.setSpeedLeft(currSpeed);
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

int getDistL() {
  unsigned long tot;      // time of travel
  unsigned int distance;
  
  digitalWrite(TRIG_L, HIGH);
  delay(10);
  digitalWrite(TRIG_L, LOW);
  tot = pulseIn(ECHO_L, HIGH);

  distance = tot/58;
  
  return distance;
}

void writeToAFile(char c){
  file = SD.open("out.txt", FILE_WRITE);
  if(file){
    int len = (int)((double)((lGap + rGap)/2)/40)*CIRC;
    file.print(len);
    file.println(c);
  }else{
    Serial.println("Could not write to a file");
  }
  file.close();
  restartGaps();
}

void restartGaps(){
  rGap = 0;
  lGap = 0;
}

void restartRot(){
  rRot = 0;
  lRot = 0;
}
