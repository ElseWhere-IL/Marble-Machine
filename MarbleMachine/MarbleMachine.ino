#define BTN_L 1
#define BTN_R 2
#define FLIP_L 3
#define FLIP_R 4

#define LDR 7

#define HALL 5
#define ELEVATOR 6

void setup() {
  // put your setup code here, to run once:
  setServo(upAngle); //TODO
}

void loop() {
  updateScreen(); // Screen refresh //TODO
  if (ballPassed()) { // Check for passing balls
    addPoint(); // Increment player points
  }
  
  rotateElevator(wheelStatus()); // Elevator rotates accordingly to wheel spin //TODO
    
  updatePinballArms(); // If player pushed buttons, activates pinball floppers
  updateBallGates(); // Randomely switches the track blocker up & down, listens to player's input ans switches the track lever
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

void wheelStatus(){
  // TODO
}

// starts and stops elevator rotation based on wheel
void rotateElevator(bool rotate){
  digitalWrite(ELEVATOR, rotate);
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

int upAngle = 45, downAngle = 135;
bool isUp = true;
int switchDelay = 4000, lastSwitched = 0;
void updateBallGates(){
  // switch correct track after delay
  if (millis() - lastSwitched >= switchDelay){

    // move gate to correct angle
    isUp = !isUp;
    if (isUp)
      setServo(upAngle); //TODO
    else
      setServo(downAngle); //TODO

    // save last time switched
    lastSwitched = millis();
  }

  // TODO: PLAYER CONTROLLED TRACK SELECTOR
}
