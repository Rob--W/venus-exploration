void startsetup(){
	venusLeft.attach(leftservo, 540, 2400);
	venusRight.attach(rightservo, 540, 2400);
	servoUltra.attach(ultraservo, 540, 2400);
	servoGrabber.attach(grabberservo, 540, 2400);

	pinMode(leftencoder, INPUT);
	pinMode(rightencoder, INPUT);
}