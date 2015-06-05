#include <Arduino.h>
#include <Servo.h>
#include "softwaredrivers.h"

#define US_BOTPIN_O 4
#define US_BOTPIN_I 5
#define mmconv				1000.0 //meter times 1000 = mm
#define IR_RB A0
#define IR_LB A1
//  Have to be defined as global variables distance/time=[mm/s]
unsigned int speedRightWheelForward = 0;
unsigned int speedLeftWheelForward = 0;
unsigned int speedRightWheelBackward = 0;
unsigned int speedLeftWheelBackward = 0;

Servo servoLeft;
Servo servoRight;
Servo servoUltra;
Servo servoGrabber;

const int leftservo = 12;
const int rightservo = 13;
const int ultraservo = 11;
const int grabberservo = 10;

unsigned int DraaiCounter;
unsigned int Delay;
unsigned int Duration;

const int leftencoder = 7;
const int rightencoder = 8;

const int pingPin = 9;

void startSetup() 
{
	servoLeft.attach(leftservo);
	servoRight.attach(rightservo);
	servoUltra.attach(ultraservo, 1300, 1700);
	servoGrabber.attach(grabberservo, 1300, 1700);

	pinMode(leftencoder, INPUT);
	pinMode(rightencoder, INPUT);
	pinMode(US_BOTPIN_O, OUTPUT);
	pinMode(US_BOTPIN_I, INPUT);

}

//
//
//void startsetup()
//{
//	venusleft.attach(leftservo, 540, 2400);
//	venusright.attach(rightservo, 540, 2400);
//	servoultra.attach(ultraservo, 540, 2400);
//	servograbber.attach(grabberservo, 540, 2400);
//
//	pinmode(leftencoder, input);
//	pinmode(rightencoder, input);
//}
//
//void turn()
//{
//	venusleft.write(120);
//	venusright.write(120);
//}
//
//
//void stop() //stop function, makes the robot stop driving (if the servo's are properly set)
//{
//	venusleft.write(90);
//	venusright.write(90);
//}
//
//void servodrive(int left, int right, int n)
//{ 
//	int pulse;
//	int prevpulse;
//	int countpulse = 0;
//
//	prevpulse = digitalread(rightencoder); //initial value encoder
//
//	venusleft.write(left);
//	venusright.write(right);
//
//	//////////////////////////////////////////////////////////////////////////
//	//																		//
//	//   continuous while loop to place scanning function in for marijn		//
//	//		vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv			//
//	//////////////////////////////////////////////////////////////////////////
//	while (n>countpulse)
//	{
//		// function executed every time to check for obstacles
//		if (checkobstacles())
//			break;
//
//		pulse = digitalread(rightencoder);
//		if (pulse != prevpulse){
//			prevpulse = pulse;
//			countpulse += 1;
//		}
//		//400 is arbitrary between  0 and 1024. this has to be tested. 
//		//the value is somewhere in between 0 and 2^8 because of a change in light and dark reception of the sensor.
//		//it will never be completly dark, that is why the value will never be zero.
//		/*if (readirlb() < 400 || readirrb() < 400)  //hole sensing code has to be added
//		{
//			break; //some kind of notification has to be added that an emergency brake has been executed.
//		}*/
//	}
//	stop();
//}
//
//void drive(int distance, int angle)  //drive function with integer parameters distance and angle in degrees
//{
//	distance = 10 * distance;
//	//turn part
//	/*angle = angle % 360;*/ //2*pirad*k=2*pirad*(k+1)
//	if (angle > 0 && angle != 180)  //turn left
//	{	
//		int turnn = round(((angle)* 100.0) / 1437.0); //number of pulses needed to turn. turns with both wheels
//		servodrive(100, 100, turnn);
//		//int turnn = ((angle * 200) / 1437); //turns with just one wheel
//		//servodrive(90, 110, turnn);
//	}
//
//	else if (angle == 180)
//	{
//		servodrive(100, 100, 13);
//	}
//
//	else   //turn right
//	{	
//		int turnn = round(((-angle) *100.0) / 1437.0); //number of pulses needed to turn. turns with both wheels
//		servodrive(80, 80, turnn);
//		//int turnn = (((360-angle) * 200) / 1437); //turns with just one wheel
//		//servodrive(90, 70, turnn);
//	}
//	//forward movement part
//	int n = round((distance*100.0) / 1317.0); //number of pulses needed to drive distance. distance in mm
//	servodrive(180, 0, n);
//}
//
//void reverse(int distance) //makes the robot drive backwards a certain distance
//{	
//	int n = ((distance*100) / 1317); //number of pulses needed to drive distance
//	serial.println(n);
//	servodrive(0, 180, n);
//}

void calibratespeedFixedDistance(int percentagePower){
	const int travelDistance = 398;	//400 millimeter
	int rightWheelTime;
	int leftWheelTime;

	int n = round((travelDistance * 16) / (66.5*PI)); //number of pulses needed to drive distance. distance in mm
	Serial.print("n: ");
	Serial.println(n);
	bool prevpulseRight = digitalRead(rightencoder);
	bool prevpulseLeft = digitalRead(leftencoder);
	int countpulseRight = 0;
	int countpulseLeft = 0;
	int speed = ((1500 + ((percentagePower) / 100.0) * 100));

	//Drive forward
	servoRight.writeMicroseconds(1500 + ((percentagePower) / 100.0) * 200);
	Serial.println(1500 + ((percentagePower) / 100.0) * 200);
	servoLeft.writeMicroseconds(1500 + ((percentagePower) / 100.0) * 200);

	int beginTime = millis();
	while ((countpulseLeft<n) || (countpulseRight<n)){
		if ((digitalRead(rightencoder) != prevpulseRight) && countpulseRight <n){
			prevpulseRight = !prevpulseRight;
			countpulseRight++;
			Serial.print("countPulse Right: ");
			Serial.println(countpulseRight);
			if (countpulseRight == n)
				rightWheelTime = (millis() - beginTime);
		}
		//counts the spokes of the left wheel
		if ((digitalRead(leftencoder) != prevpulseLeft) && countpulseLeft <n){
			prevpulseLeft = !prevpulseLeft;
			countpulseLeft++;
			Serial.print("countPulse Left: ");
			Serial.println(countpulseLeft);
			if (countpulseLeft == n)
				leftWheelTime = (millis() - beginTime);
		}
	}
	stop();

	speedLeftWheelForward = round((travelDistance*mmconv) / leftWheelTime); //speed = travel/time ; travel = (countpulseLeft/16)*66.5*pi;
	Serial.print("leftWheelTime: ");
	Serial.print(leftWheelTime);
	Serial.print("	FLeft:	");
	Serial.println(speedLeftWheelForward);
	speedRightWheelBackward = round((travelDistance*mmconv) / rightWheelTime);
	Serial.print("rightWheelTime: ");
	Serial.print(rightWheelTime);
	Serial.print("	BRight:	");
	Serial.println(speedRightWheelBackward);


	//reinitialising variables
	prevpulseRight = digitalRead(rightencoder);
	prevpulseLeft = digitalRead(leftencoder);
	countpulseRight = 0;
	countpulseLeft = 0;
	rightWheelTime = -1;
	leftWheelTime = -1;

	//Drive backward
	servoRight.writeMicroseconds(1500 - ((percentagePower) / 100.0) * 200);
	Serial.println(1500 - ((percentagePower) / 100.0) * 200);
	servoLeft.writeMicroseconds(1500 - ((percentagePower) / 100.0) * 200);

	beginTime = millis();
	while ((countpulseLeft<n) || (countpulseRight<n)){
		//counts the spokes of the right wheel
		if ((digitalRead(rightencoder) != prevpulseRight) && countpulseRight<n){
			prevpulseRight = !prevpulseRight;
			countpulseRight++;
			if (countpulseRight == n){
				rightWheelTime = (millis() - beginTime);
				servoRight.write(90);
			}
		}
		//counts the spokes of the left wheel
		if ((digitalRead(leftencoder) != prevpulseLeft) && countpulseLeft<n){
			prevpulseLeft = !prevpulseLeft;
			countpulseLeft++;
			if (countpulseLeft == n)
				leftWheelTime = (millis() - beginTime);
		}
	}
	stop();

	speedLeftWheelBackward = round((travelDistance*mmconv) / leftWheelTime); //speed = travel/time ; travel = (countpulseLeft/16)*66.5*pi
	Serial.print("leftWheelTime: ");
	Serial.print(leftWheelTime);
	Serial.print("	BLeft:	");
	Serial.println(speedLeftWheelBackward);
	speedRightWheelForward = round((travelDistance*mmconv) / rightWheelTime);
	Serial.print("rightWheelTime: ");
	Serial.print(rightWheelTime);
	Serial.print("FRight:	");
	Serial.println(speedRightWheelForward);

}

int calculateAngleDelayLeft(int angle){
	//Serial.print(((((105 * PI*angle) / (360 * speedRightWheelForward)) + ((105 * PI*angle) / (360 * speedRightWheelForward))) / 2) * 1000); // time for right wheel and left wheel averaged.(tR+tL)/2
	//return(((((105 * PI*angle) / (360 * speedRightWheelForward)) + ((105 * PI*angle) / (360 * speedRightWheelForward))) / 2)*1000);
	Serial.print(((105 * PI*angle) / 180) / (speedRightWheelBackward + speedLeftWheelBackward)*mmconv);
	return((((105 * PI*angle) / 180) / (speedRightWheelBackward + speedLeftWheelBackward))*mmconv); //average speed instead of time (seems to be more exact)
}

int calculateAngleDelayRight(int angle){
	//Serial.print(((((105 * PI*angle) / (360 * speedRightWheelForward)) + ((105 * PI*angle) / (360 * speedRightWheelForward))) / 2) * 1000); // time for right wheel and left wheel averaged.(tR+tL)/2
	//return(((((105 * PI*angle) / (360 * speedRightWheelForward)) + ((105 * PI*angle) / (360 * speedRightWheelForward))) / 2)*1000);
	Serial.print(((105 * PI*angle) / 180) / (speedLeftWheelBackward + speedRightWheelBackward)*mmconv);
	return((((105 * PI*angle) / 180) / (speedLeftWheelBackward + speedRightWheelBackward))*mmconv);
}

int calculateDistanceDelayBackward(int distance){ //distance in mm
	return((2*distance*mmconv)/(speedRightWheelBackward+speedLeftWheelForward));
	//Serial.println(((distance*mmconv) / 2)*((1.0 / speedRightWheelBackward) + (1.0 / speedLeftWheelForward)));
	//return(((distance*mmconv) / 2)*((1.0 / speedRightWheelBackward) + (1.0 / speedLeftWheelForward)));
}

int calculateDistanceDelayForward(int distance){ //distance in mm
	return((2 * distance*mmconv) / (speedLeftWheelBackward + speedRightWheelForward));
	//return(((distance*mmconv) / (2 * speedLeftWheelBackward)) + ((distance*mmconv) / (2 * speedRightWheelBackward)));
	//return(((distance*mmconv) / 2)*((1.0 / speedLeftWheelBackward) + (1.0 / speedRightWheelForward)));
}

void turnLeft(int percentagePower, int angle){
	servoLeft.write(90 - ((percentagePower / 100.0) * 90));
	servoRight.write(90 - ((percentagePower / 100.0) * 90));
	delay(calculateAngleDelayLeft(angle));
}

void turnRight(int percentagePower, int angle){
	servoLeft.write(90 + ((percentagePower / 100.0) * 90));
	servoRight.write(90 + ((percentagePower / 100.0) * 90));
	delay(calculateAngleDelayRight(angle));
}

int Backward(int percentagePower, int distance){ //PERCENTAGEpower is to regulate tire speed, keep at 90. returns distance in centimeter.
	int i = 0;
	int countpulse = 0;
	bool prevpulse = digitalRead(rightencoder);
	long t = calculateDistanceDelayBackward(distance);
	Serial.println(t);
	//Drive forward
	servoRight.write(90 + (90 * (percentagePower / 100.0)));
	servoLeft.write(90 - (90 * (percentagePower / 100.0)));

	long travelTime = millis() + t;
	while (travelTime>millis()){
		//fun code
		//IRscan;
		//Mappen;
		//Ultratopread
		if (checkObstacles())
			break;

		//Spakentellen/Count spokes to find the distance traveled (approximately
		if (digitalRead(rightencoder) != prevpulse){
			prevpulse = !prevpulse;
			countpulse++;
		}
	}
	stop();
	return (countpulse*13.7);
}

int philipsForward(int percentagePower, int distance){ //PERCENTAGEpower is to regulate tire speed, keep at 90. returns distance in centimeter.
	int i = 0;
	int countpulse = 0;
	bool prevpulse = digitalRead(rightencoder);
	long t = calculateDistanceDelayForward(distance);
	Serial.println(t);
	//Drive forward
	servoRight.write(90 - (90 * (percentagePower / 100.0)));
	servoLeft.write(90 + (90 * (percentagePower / 100.0)));

	long travelTime = millis() + t;
	while (travelTime>millis()){
		//fun code
		//IRscan;
		//Mappen;
		//Ultratopread
		if (checkObstacles())
			break;

		//Spakentellen/Count spokes to find the distance traveled (approximately
		if (digitalRead(rightencoder) != prevpulse){
			prevpulse = !prevpulse;
			countpulse++;
		}
	}
	stop();
	return (countpulse*13.7);
}

int drive(unsigned int distance, int angle)
{
	if (angle >= 0) {
		// left angle
		turnLeft(100, angle);
		//PLeft(abs(angle) * round(555 / 90));
	} else if (angle < 0) {
		// right angle
		turnRight(100, angle);
		//PRight(abs(angle) * round(555 / 90));
	} 

	return philipsForward(100, distance*10);
		//FForward(distance * round(2000.0 / 30));
}

// Full speed forward
int FForward(long distanceDelay) {
	// Traveled distance (in cm).
	int i = 0;
	int countpulse = 0;
	bool prevpulse = digitalRead(rightencoder);
	servoLeft.writeMicroseconds(1700);         // Left wheel counterclockwise
	servoRight.writeMicroseconds(1300);        // Right wheel clockwise

	Serial.println(distanceDelay);

	if (distanceDelay != 0) {
		//delay(distanceDelay);       
		/*for (; i < distanceDelay; ++i) {
			// Loops a distance amount of time, 66.666ms per cm
			if (checkObstacles())
				//break;
			//Serial.println("spam");
			delay(2000 / 30);
		}*/
		//"millis-delay";
		long t = (millis() + distanceDelay);	//Timestamp + time wallee has to drive
		while (millis() < (t)){		//while loop that will stay in it for a certain time distanceDelay
			//IRscan;
			//Mappen;
			//Ultratopread

			
			if (checkObstacles())
				break;
			
			//Spakentellen/Count spokes to find the distance traveled (approximately
			if (digitalRead(rightencoder) != prevpulse){
				prevpulse = !prevpulse;
				countpulse++;
			}
		}

	}
	stop();
	//return (countpulse*distance per spoke=13.7); //return needed for the millis()-delay
	return (countpulse*13.7);
}

// Turn left in place
void PLeft(int angleDelay){
	servoLeft.writeMicroseconds(1300);         // Left wheel clockwise
	servoRight.writeMicroseconds(1300);        // Right wheel clockwise
	delay(angleDelay);                                // ...for 0.6 seconds (er stond 600)
}

// Turn right in place
void PRight(int angleDelay){
	servoLeft.writeMicroseconds(1700);         // Left wheel counterclockwise
	servoRight.writeMicroseconds(1700);        // Right wheel counterclockwise
	delay(angleDelay);                                // ...for 0.6 seconds ( er stond 600)
}
// Full speed backward
void FBack(long distanceDelay){
	servoLeft.writeMicroseconds(1300);         // Left wheel clockwise
	servoRight.writeMicroseconds(1700);        // Right wheel counterclockwise
	delay(distanceDelay);                               // ...for 2 seconds
}

void Turn180() {
	servoLeft.writeMicroseconds(1700);
	servoRight.writeMicroseconds(1700);
	delay(1075);

	stop();
}

void stop() //Stop function, makes the robot stop driving (if the servo's are properly set)
{
	servoLeft.writeMicroseconds(1500);         // Pin 13 stay still
	servoRight.writeMicroseconds(1500);        // Pin 12 stay still
}


int readIRMid() //reads Middle IR. Return unit to be determined
{

}

int readIRGrab()    //Reads IR Grabber. Return unit to be determined;
{		

}

int readLightSense()                   //Reads Light Sensor. Return unit to be determined
{	

}

colour readirlb()                         //reads ir left bottom. return unit to be determined; Threshold: 950 is black. grey 890, black 990, white < 800
{
	int temp = analogRead(IR_LB);
	if (temp > 950)
		return BLACK;
	else if (temp > 800 && temp < 950)
		return GREY;
	else if (temp < 800)
		return WHITE;
	else
		return OTHER;
}


colour readirrb()                         //reads ir right bottom. return unit to be determined;  Threshold: 950 is black. grey 890, black 990, white < 800
{
	int temp = analogRead(IR_RB);
	if (temp > 950)
		return BLACK;
	else if (temp > 800 && temp < 950)
		return GREY;
	else if (temp < 800)
		return WHITE;
	else
		return OTHER;
}

int microsecondsToCentimeters(int microseconds)
{
	// The speed of sound is 340 m/s or 29 microseconds per centimeter.
	// The ping travels out and back, so to find the distance of the
	// object we take half of the distance travelled.
	return microseconds / 29 / 2;
}

int readUltraTop(int angle)  //Reads Top Ultrasonic sensor. Return unit is a distance. Input parameter is a angle; full left is angle 0 degrees full right is 180 degrees
{		
	servoUltra.write(180 - angle);
	// establish variables for duration of the ping, 
	// and the distance result in inches and centimeters:
	int duration, inches, cm;

	// The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
	// Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
	pinMode(pingPin, OUTPUT);
	digitalWrite(pingPin, LOW);
	delayMicroseconds(2);
	digitalWrite(pingPin, HIGH);
	delayMicroseconds(5);
	digitalWrite(pingPin, LOW);

	// The same pin is used to read the signal from the PING))): a HIGH
	// pulse whose duration is the time (in microseconds) from the sending
	// of the ping to the reception of its echo off of an object.
	pinMode(pingPin, INPUT);
	duration = pulseIn(pingPin, HIGH);

	// convert the time into a distance
	cm = microsecondsToCentimeters(duration);
	/*
	Serial.print(inches);
	Serial.print("in, ");
	Serial.print(cm);
	Serial.print("cm");
	Serial.println();
	*/
	delay(100);
	return cm;
}

int readUltraBot()
{
	// establish variables for duration of the ping, 
	// and the distance result in inches and centimeters:
	int duration, inches, cm;

	// The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
	// Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
	digitalWrite(US_BOTPIN_O, LOW);
	delayMicroseconds(2);
	digitalWrite(US_BOTPIN_O, HIGH);
	delayMicroseconds(5);
	digitalWrite(US_BOTPIN_O, LOW);

	// The same pin is used to read the signal from the PING))): a HIGH
	// pulse whose duration is the time (in microseconds) from the sending
	// of the ping to the reception of its echo off of an object.
	duration = pulseIn(US_BOTPIN_I, HIGH);

	// convert the time into a distance
	cm = microsecondsToCentimeters(duration);

	//Serial.print(inches);
	//Serial.print("in, ");
	//Serial.print(cm);
	//Serial.print("cm");
	//Serial.println();

	delay(100);
	return cm;
}
void openGrabber()
{
	//servoGrabber.attach(grabberservo);
	servoGrabber.write(0);
	//servoGrabber.detach();
}
void closeGrabber()
{
	// servoGrabber.attach(grabberservo);
	servoGrabber.write(180);
	//servoGrabber.detach();
}
