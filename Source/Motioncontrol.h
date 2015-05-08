void closeGrabber(){
 // servoGrabber.attach(grabberservo);
  servoGrabber.write(180);
  //servoGrabber.detach();
}

void openGrabber(){
  //servoGrabber.attach(grabberservo);
  servoGrabber.write(0);
  //servoGrabber.detach();
}

void lookAhead(){
  // servoUltra.attach(grabberservo);
  servoUltra.write(90);
  //servoUltra.detach();
}

void lookLeft(){
 // servoUltra.attach(grabberservo);
  servoUltra.write(180);
  //servoUltra.detach();
}

void lookRight(){
 // servoUltra.attach(grabberservo);
  servoUltra.write(0);
  //servoUltra.detach();
}

void lookAngle(int angle){// an angle of 180 means it wil look to its left, an angle of 0 means it will look to its right, an angle of 90 means it will look straight(-ish) ahead
  // servoUltra.attach(grabberservo);
  servoUltra.write(angle);
  //servoUltra.detach();
}

void goForward(){
  venusLeft.write(180);
  venusRight.write(0);
}
void goStop(){
    venusLeft.write(90);
    venusRight.write(90);  
}

void goBack(){
  venusLeft.write(0);
  venusRight.write(180);
}

void turnLeft(){//needs calculations with the digital encoder on the wheels to give a more controlled value of turning angle
  venusLeft.write(180);
  venusRight.write(180);
}

void turnRight(){//needs calculations with the digital encoder on the wheels to give a more controlled value of turning angle
  venusLeft.write(0);
  venusRight.write(0);
}

boolean detectLeftEncoderState(){
 return digitalRead(leftencoder);
  
}
int detectLeftEncoderPulses(boolean reading){
  boolean leftEncoderState = detectLeftEncoderState();
  if (reading != leftEncoderState) {
    reading = leftEncoderState;
    return 1;
  }
  return 0;
}
void scan();

void moveForward(double distance){
  int givenPulses;
  int inputPulses = 0;
  boolean reading = detectLeftEncoderState();
  givenPulses = (distance) / 1.3;
  detectLeftEncoderState();
  while(givenPulses>inputPulses){
    goForward();
    inputPulses = detectLeftEncoderPulses(reading) + inputPulses;
    
  }
  goStop();
}
  
void moveRotate(double angle){
  int givenPulses;
  int inputPulses = 0;
  givenPulses = angle/14.116;
  boolean reading = detectLeftEncoderState();
  while(givenPulses> inputPulses){
    turnLeft();
    inputPulses = detectLeftEncoderPulses(reading) + inputPulses;
  }
}