#include <Arduino.h>
#include <Servo.h>
#include "softwaredrivers.h"

#define US_BOTPIN_O 4
#define US_BOTPIN_I 5
#define mmconv				1000.0 //meter times 1000 = mm
#define IR_RB A0
#define IR_LB A1
#define motorpower	90		//define percentagepower in drive/philipsforward function

//  Have to be defined as global variables distance/time=[mm/s];	
//Initial value is actually to low, but it is to make sure that it does not divide by zero on our performance.
//Forward and backward are defined reletive to the front (grabber) of the robot.
unsigned int speedRightWheelForward = 140;
unsigned int speedLeftWheelForward = 140;
unsigned int speedRightWheelBackward = 140;
unsigned int speedLeftWheelBackward = 140;

Servo servoLeft;
Servo servoRight;
Servo servoUltra;
Servo servoGrabber;

#define leftservo	 12
#define rightservo	 13
#define ultraservo	 11
#define grabberservo 10

#define leftencoder	 7
#define rightencoder 8
#define pingPin		 9
 


void startSetup() 
{
	servoLeft.attach(leftservo, 1300, 1700);
	servoRight.attach(rightservo, 1300, 1700);
	servoUltra.attach(ultraservo, 750, 2250);
	servoGrabber.attach(grabberservo, 750, 2250);

	pinMode(leftencoder, INPUT);
	pinMode(rightencoder, INPUT);
	pinMode(US_BOTPIN_O, OUTPUT);
	pinMode(US_BOTPIN_I, INPUT);
}

void ServoUturn(int angle){
	servoUltra.write(angle);
}

void calibratespeedFixedDistance(int percentagePower){
	const int travelDistance = 396;	//400 millimeter
	int rightWheelTime;
	int leftWheelTime;

	int n = 31;//round((travelDistance * 16) / (66.5*PI)); //number of pulses needed to drive distance. distance in mm
	bool prevpulseRight = digitalRead(rightencoder);
	bool prevpulseLeft = digitalRead(leftencoder);
	int countpulseRight = 0;
	int countpulseLeft = 0;

	//Drive forward
	//This can be re written to writeMicroseconds(1700) (100%) writeMicroseconds(1680) (90%) depending on power
	servoRight.writeMicroseconds(round(1500 + ((percentagePower) / 100.0) * 200));	//right backward
	servoLeft.writeMicroseconds(round(1500 + ((percentagePower) / 100.0) * 200));	//left forward

	int beginTime = millis();
	while ((countpulseLeft<n) || (countpulseRight<n)){
		if ((digitalRead(rightencoder) != prevpulseRight) && countpulseRight <n){
			prevpulseRight = !prevpulseRight;
			countpulseRight++;
			if (countpulseRight == n)
				rightWheelTime = (millis() - beginTime);
		}
		//counts the spokes of the left wheel
		if ((digitalRead(leftencoder) != prevpulseLeft) && countpulseLeft <n){
			prevpulseLeft = !prevpulseLeft;
			countpulseLeft++;
			if (countpulseLeft == n)
				leftWheelTime = (millis() - beginTime);//speed*WheelForward/backward can be placed here to save space
		}
	}
	stop();

	speedLeftWheelForward = round((travelDistance*mmconv) / leftWheelTime); //speed = travel/time ; travel = (countpulseLeft/16)*66.5*pi;
	speedRightWheelBackward = round((travelDistance*mmconv) / rightWheelTime);


	//reinitialising variables
	prevpulseRight = digitalRead(rightencoder);
	prevpulseLeft = digitalRead(leftencoder);
	countpulseRight = 0;
	countpulseLeft = 0;
	rightWheelTime = -1;
	leftWheelTime = -1;

	//Drive backward
	servoRight.writeMicroseconds(round(1500 - ((percentagePower) / 100.0) * 200));	//right forward
	servoLeft.writeMicroseconds(round(1500 - ((percentagePower) / 100.0) * 200));	//left backward

	beginTime = millis();
	while ((countpulseLeft<n) || (countpulseRight<n)){
		//counts the spokes of the right wheel
		if ((digitalRead(rightencoder) != prevpulseRight) && countpulseRight<n){
			prevpulseRight = !prevpulseRight;
			countpulseRight++;
			if (countpulseRight == n){
				rightWheelTime = (millis() - beginTime);
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
	speedRightWheelForward = round((travelDistance*mmconv) / rightWheelTime);

}

int calculateAngleDelayLeft(int angle){	//counterclockwise turning (tegen de klok in als je van boven naar de grond kijkt)
	return round((((105 * PI*angle) / 180) / (speedRightWheelForward + speedLeftWheelBackward))*mmconv); //average speed instead of time (seems to be more accurate)
}

int calculateAngleDelayRight(int angle){	//clockwise turning (met de klok mee in als je van boven naar de grond kijkt)
	return round((((105 * PI*angle) / 180) / (speedLeftWheelForward + speedRightWheelBackward))*mmconv);	//average speed instead of time (seems to be more accurate)
}

int calculateDistanceDelayBackward(int distance){ //distance in mm
	return round((2*distance*mmconv)/(speedRightWheelBackward+speedLeftWheelBackward));
}

int calculateDistanceDelayForward(int distance){ //distance in mm
	return round((2 * distance*mmconv) / (speedLeftWheelForward + speedRightWheelForward));
}

void turnLeft(int percentagePower, int angle){
	servoLeft.write(round(90 - ((percentagePower / 100.0) * 90)));
	servoRight.write(round(90 - ((percentagePower / 100.0) * 90)));
	delay(calculateAngleDelayLeft(angle));
}

void turnRight(int percentagePower, int angle){
	servoLeft.write(round(90 + ((percentagePower / 100.0) * 90)));
	servoRight.write(round(90 + ((percentagePower / 100.0) * 90)));
	delay(calculateAngleDelayRight(angle));
}

int Backward(int percentagePower, int distance){ //PERCENTAGEpower is to regulate tire speed, keep at 90. returns distance in centimeter.
	int countpulse = 0;
	bool prevpulse = digitalRead(rightencoder);
	//Drive forward
	servoRight.write(round(90 + (90 * (percentagePower / 100.0))));
	servoLeft.write(round(90 - (90 * (percentagePower / 100.0))));

	long travelTime = millis() + calculateDistanceDelayBackward(distance);
	//this part is used in multiple functions. can be shortend
	while (travelTime>millis()){
		//fun code
		if (checkObstacles())
			break;

		//Spakentellen/Count spokes to find the distance traveled (approximately
		if (digitalRead(rightencoder) != prevpulse){
			prevpulse = !prevpulse;
			countpulse++;
		}
	}
	stop();
	return round(countpulse*13.7);
}

int philipsForward(int percentagePower, int distance){ //PERCENTAGEpower is to regulate tire speed, keep at 90. returns distance in centimeter.
	int countpulse = 0;
	bool prevpulse = digitalRead(rightencoder);
	//Drive forward
	servoRight.write(round(90 - (90 * (percentagePower / 100.0))));
	servoLeft.write(round(90 + (90 * (percentagePower / 100.0))));

	long travelTime = millis() + calculateDistanceDelayForward(distance);
	while (travelTime>millis()){
		//fun code
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
		turnLeft(motorpower, angle);
		//PLeft(abs(angle) * round(555 / 90));
	} else if (angle < 0) {
		// right angle
		turnRight(motorpower, -angle);
		//PRight(abs(angle) * round(555 / 90));
	} 

	return philipsForward(motorpower, distance*100);
		//FForward(distance * round(2000.0 / 30));
}

void Turn180() {
	servoLeft.writeMicroseconds(1700);
	servoRight.writeMicroseconds(1700);
	delay(1075);

	stop();
}

void stop(){ //Stop function, makes the robot stop driving (if the servo's are properly set)
	servoLeft.writeMicroseconds(1500);         // Pin 13 stay still
	servoRight.writeMicroseconds(1500);        // Pin 12 stay still
}

void calibrateIRSensor()
{
	// Safety: if zwart != 0
	//             Break;

	// If (read is niet wit) 
	// read is zwart

	// Voor grijs moet er in de read functies aangepast worden: if read != Wit en != Zwart return Grijs;
}

int readIRMid(){ //reads Middle IR. Return unit to be determined

}

colour readIRGrab(){    //Reads IR Grabber. Return unit to be determined;
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

int readLightSense(){                 //Reads Light Sensor. Return unit to be determined

}

colour readIRLB()                         //reads ir left bottom. return unit to be determined; Threshold: 950 is black. grey 890, black 990, white < 800
{
	int temp = analogRead(IR_LB);
	if (temp > 950)
		return BLACK;
	else if (temp > 800 && temp < 950)
		return GREY;
	else if (temp > 950)
		return WHITE;
	else
		return OTHER;
}

colour readIRRB()                         //reads ir right bottom. return unit to be determined;  Threshold: 950 is black. grey 890, black 990, white < 800
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

unsigned int microsecondsToCentimeters(unsigned int microseconds)
{
	// The speed of sound is 340 m/s or 29 microseconds per centimeter.
	// The ping travels out and back, so to find the distance of the
	// object we take half of the distance travelled.
	return microseconds / 29 / 2;
}

unsigned int readUltraTop(int angle)  //Reads Top Ultrasonic sensor. Return unit is a distance. Input parameter is a angle; full left is angle 0 degrees full right is 180 degrees
{		
	servoUltra.write(180 - angle);
	// establish variables for duration of the ping, 
	// and the distance result in centimeters:
	unsigned int duration, cm;

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
	Serial.print(cm);
	Serial.print("cm");
	Serial.println();
	*/
	delay(100);
	return cm;
}

unsigned int readUltraBot()
{
	// establish variables for duration of the ping, 
	// and the distance result in centimeters:
	unsigned int duration, cm;

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

	//Serial.print(cm);
	//Serial.print("cm");
	//Serial.println();

	delay(100);
	if (cm < 2) {
		// This is not possible, but when it happens, return 0 instead of
		// an underflown number.
		return 0;
	}
	return cm - 2;
}

void openGrabber(){
	servoGrabber.attach(grabberservo, 750, 2250);
	servoGrabber.write(0);
	delay(500);
	servoGrabber.detach();
}

void closeGrabber(){
	servoGrabber.attach(grabberservo, 750, 2250);
	servoGrabber.write(180);
	delay(1000);
}
