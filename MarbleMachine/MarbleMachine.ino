#include <Servo.h>
#include <RGBmatrixPanel.h>
#include <DFRobotDFPlayerMini.h>
#include <SoftwareSerial.h>

#define BTN_L 1
#define BTN_R 2
#define FLIP_L 3
#define FLIP_R 4

#define LDR 7

#define HALL 5
#define ELEVATOR 6
#define TESTLED 6

#define TRACK_SELECT 8

// SCREEN
#define CLK 11
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);

SoftwareSerial speakerSerial(12, 13); // RX, TX
DFRobotDFPlayerMini speaker;

Servo wall, track;
int gameTime = 90;
int points = 0;
int startTime = 0, timeLeft = 1, lastTimeLeft = 0; // set timeLeft to 1 as loop only works if timeLeft > 0

bool playing = false;

void setup() {
  Serial.begin(9600);
  
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
  pinMode(TESTLED, OUTPUT);
  
  //SERVOS
  wall.attach(10);
  track.attach(8);
  
  setWall(true);
  setTrack(true);

  matrix.begin();

  speakerSerial.begin(9600);
  speaker.begin(speakerSerial);
  speaker.volume(10);
}

void loop() {
  if (playing){
    timeLeft = (gameTime - (millis() - startTime)) / 1000; // calc time remaining

    if (timeLeft != lastTimeLeft) // time left changed, update screen
      updateScreen();
      
    if (ballPassed()) { // Check for passing balls
      addPoint(); // Increment player points
    }
    
    rotateElevator(wheelStatus()); // Elevator rotates accordingly to wheel spin
      
    updatePinballArms(); // If player pushed buttons, activates pinball floppers
    updateMovingWall(); // Randomely switches the track blocker up & down, listens to player's input ans switches the track lever
  
    /*int wspeed = wheelStatus();
    analogWrite(TESTLED, wspeed);*/
  
    lastTimeLeft = timeLeft;

    // end game when timer runs out
    if (timeLeft <= 0){
      playing = false;
  
      speaker.play(2); // play game over sound;
      updateScreen();
    }
  } else{
    playing = checkStartGame(); // check if player starts game
  }


}

bool checkStartGame(){
  //TODO
}

void updateScreen(){
  int secLeft = timeLeft / 1000;
  
  matrix.fillScreen(matrix.Color333(0,0,0)); // clear screen

  // draw titles
  matrix.setTextSize(1);
  matrix.setCursor(0,0);
  matrix.print("TIME");

  matrix.setCursor(32, 0);
  matrix.print("SCORE");

  // draw time
  matrix.setTextSize(2);
  
  matrix.setCursor(0, 16);
  if (secLeft < 10)
    matrix.print(0);
  matrix.print(secLeft);

  // draw score
  matrix.setCursor(32, 16);
  if (points < 10)
    matrix.print(0);
  matrix.print(points);
}

bool lastLDRState = true;
bool ballPassed(){
  bool LDRState = digitalRead(LDR);
  bool passed = !LDRState && lastLDRState; //laser is blocked and wasn't blocked before
  
  lastLDRState = LDRState;
  return passed;
}

void addPoint(){
  points++;
  
  speaker.play(1); // play point sound
  updateScreen();
  
  //TODO: effect
}

const byte NumMagnets = 2;
const float MaxSpeed = 2;
const int wheelStopTime = 2000;

int lastSlowTime;

bool wasMagnet;
unsigned long lastMagnetTime;
float wheelSpeed;
float wheelStatus(){  
  bool isMagnet = !digitalRead(HALL); 
  int timeSinceMagnet = (millis() - lastMagnetTime);
  
  if (isMagnet && !wasMagnet){
    float perSec = 1 / (NumMagnets * timeSinceMagnet / 1000.0);
    wheelSpeed = perSec/MaxSpeed * 255;
    lastMagnetTime = millis();
  } else if (timeSinceMagnet >= 1000 && millis() - lastSlowTime >= wheelStopTime / 255){ // constantly slow down wheel
    wheelSpeed = max(0, wheelSpeed-1);
    lastSlowTime = millis();
  }

  wasMagnet = isMagnet;
  return min(wheelSpeed, 255);
}

// starts and stops elevator rotation based on wheel
void rotateElevator(int spd){
  analogWrite(ELEVATOR, spd);
}

// returns true if button is pressed
// might need to invert return value
// depends on button implementation
bool buttonState(int button){
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
  if (timeSinceFlipL >= flipDelay && btnLeft == true && lastBtnLeft == false){
    digitalWrite(FLIP_L, HIGH);
    lastFlippedL = millis();
  }
  if (timeSinceFlipR >= flipDelay && btnRight == true && lastBtnRight == false){
    digitalWrite(FLIP_R, HIGH);
    lastFlippedR = millis();  
  }

  // save previous button states
  lastBtnLeft = btnLeft;
  lastBtnRight = btnRight;
}

int upAngleWall = 90, downAngleWall = 150;
int upAngleTrack = 45, downAngleTrack = 135;
void setWall(bool up){
  if (up)
    wall.write(upAngleWall);
   else
    wall.write(downAngleWall);
}

void setTrack(bool up){
  if (up)
    track.write(upAngleTrack);
   else
    track.write(downAngleTrack);
}

bool isUp = true;
int switchDelay = 4000;
unsigned long lastSwitched = 0;
void updateMovingWall(){
  // switch correct track after delay
  if (millis() - lastSwitched >= switchDelay){
    Serial.println(lastSwitched);
    // save last time switched
    lastSwitched = millis();
    
    // move gate to correct angle
    isUp = !isUp;
    setWall(isUp);
  }
}

void updateTrackSelector(){
  setTrack(digitalRead(TRACK_SELECT));
}
