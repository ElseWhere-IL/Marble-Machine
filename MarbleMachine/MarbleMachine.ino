#include <Servo.h>

#define BTN_L 1
#define BTN_R 2
#define FLIP_L 3
#define FLIP_R 4

#define LDR 7

#define HALL 5
#define ELEVATOR 6

#define TRACK_SELECT 9

Servo wall, track;

void setup() {

  //BUTTONS
  pinMode(BTN_L, INPUT);
  pinMode(BTN_R, INPUT);

  //FLIPPERS
  pinMode(FLIP_L, OUTPUT);
  pinMode(FLIP_R, OUTPUT);

  //MISC
  pinMode(LDR, INPUT);
  pinMode(HALL, INPUT);
  pinMode(ELEVATOR, OUTPUT);
  pinMode(TRACK_SELECT, OUTPUT);
  
  //SERVOS
  wall.attach(7);
  track.attach(8);
  
  setWall(true);
  setTrack(true);
}

void loop() {
  updateScreen(); // Screen refresh //TODO
  if (ballPassed()) { // Check for passing balls
    addPoint(); // Increment player points
  }
  
  rotateElevator(wheelStatus()); // Elevator rotates accordingly to wheel spin
    
  updatePinballArms(); // If player pushed buttons, activates pinball floppers
  updateMovingWall(); // Randomely switches the track blocker up & down, listens to player's input ans switches the track lever
}

bool lastLDRState = true;
void ballPassed(){
  bool LDRState = digitalRead(LDR);
  bool passed = !LDRState && lastLDRState; //laser is blocked and wasn't blocked before
  
  lastLDRState = LDRState;
  return passed;
}

int points = 0;
void addPoint(){
  points++;
  //TODO: effect
}

const unsigned byte NumMagnets = 5;
const float MaxSpeed = 3.5;

bool wasMagnet;
unsigned long lastMagnetTime
float wheelSpeed;
void wheelStatus(){  
  bool isMagnet = digitalRead(HALL); 

  if (isMagnet && !notMagnet){
    float perSec = (millis() - lastMagnetTime)*5 / 1000;
    wheelSpeed = map(perSec, 0, MaxSpeed, 0, 255);
    lastMagnetTime = millis();
  }

  wasMagnet = isMagnet;
  return wheelSpeed;
}

// starts and stops elevator rotation based on wheel
void rotateElevator(int spd){
  analogWrite(ELEVATOR, spd);
}

// returns true if button is pressed
// might need to invert return value
// depends on button implementation
bool buttonStatus(int button){
  return digitalRead(button);
}

bool lastBtnLeft = false, lastBtnRight = false;
int lastFlippedL = 0, lastFlippedR = 0;
int flipDuration = 50, flipDelay = 100;
void updatePinballArms(){
  // get fliiper button states
  bool btnLeft = buttonState(BTN_L), btnRight = buttonState(BTN_R);
  int timeSinceFlipL = millis() - lastFlippedL;
  int timeSinceFlipR = millis() - lastFlippedR;
  
  // turn flippers off after a duration
  if (timeSinceFlipL >= flipDuration)
    digitalWrite(FLIP_L, LOW);
  if (timeSinceFlipL >= flipDuration)
    digitalWrite(FLIP_R, LOW);

  // turn flippers on if button is pressed and delay has passed
  if (timeSinceFlipL >= flipDelay, btnLeft == true && lastBtnLeft = false){
    digitalWrite(FLIP_L, HIGH);
    lastFlippedL = millis();
  }
  if (timeSinceFlipR >= flipDelay, btnRight == true && lastBtnRight = false){
    digitalWrite(FLIP_R, HIGH);
    lastFlippedR = millis();  
  }

  // save previous button states
  lastBtnLeft = btnLeft;
  lastBtnRight = btnRight;
}

int upAngleWall = 45, downAngleWall = 135;
int upAngleTrack = 45, downAngleTrack = 135;
void setWall(bool up){
  if (up)
    wall.set(upAngleWall);
   else
    wall.set(downAngleWall);
}

void setTrack(bool up){
  if (up)
    track.set(upAngleTrack);
   else
    track.set(downAngleTrack);
}

bool isUp = true;
int switchDelay = 4000, lastSwitched = 0;
void updateMovingWall(){
  // switch correct track after delay
  if (millis() - lastSwitched >= switchDelay){

    // move gate to correct angle
    isUp = !isUp;
    setWall(isUp);
    
    // save last time switched
    lastSwitched = millis();
  }

  // TODO: PLAYER CONTROLLED TRACK SELECTOR
}

void updateTrackSelector(){
  setTrack(digitalRead(TRACK_SELECT));
}
