Servo venusLeft;
Servo venusRight;
Servo servoUltra;
Servo servoGrabber;

int leftservo = 12;
int rightservo = 13;
int ultraservo = 11;
int grabberservo = 10;

//Motion-wheels

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

//Servo-grabber

void openGrabber(){
	//servoGrabber.attach(grabberservo);
	servoGrabber.write(0);
	//servoGrabber.detach();
}
void closeGrabber(){
	// servoGrabber.attach(grabberservo);
	servoGrabber.write(180);
	//servoGrabber.detach();
}

//Servo-Ultra

void lookAhead(){
	servoUltra.write(90);
}

void lookLeft(){
	servoUltra.write(180);
}

void lookRight(){
	servoUltra.write(0);
}

void lookAngle(int angle){// an angle of 180 means it wil look to its left, an angle of 0 means it will look to its right, an angle of 90 means it will look straight(-ish) ahead
	servoUltra.write(angle);
}

