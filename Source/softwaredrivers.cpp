#include <Arduino.h>
#include <Servo.h>
#include "softwaredrivers.h"


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
	servoUltra.attach(ultraservo, 540, 2400);
	servoGrabber.attach(grabberservo, 540, 2400);

	pinMode(leftencoder, INPUT);
	pinMode(rightencoder, INPUT);

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

int drive(unsigned int distance, int angle)
{
	if (angle < 0) {
		// left angle
		PLeft(abs(angle) * round(555 / 90));
	} else if (angle > 0) {
		// right angle
		PRight(abs(angle) * round(555 / 90));
	}
	return FForward(distance * round(2000 / 30));
}

// Full speed forward
int FForward(int distanceDelay) {
	// Traveled distance (in cm).
	int i = 0;
	if (distanceDelay != 0) {
		servoLeft.writeMicroseconds(1700);         // Left wheel counterclockwise
		servoRight.writeMicroseconds(1300);        // Right wheel clockwise
		delay(distanceDelay);       
		for (; i < distanceDelay && checkObstacles(); ++i) {
			// Loops a distance amount of time, 66.666ms per cm
			delay(2000 / 30);
		}
	}
	stop();
	return i;
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
void FBack(int distanceDelay){
	servoLeft.writeMicroseconds(1300);         // Left wheel clockwise
	servoRight.writeMicroseconds(1700);        // Right wheel counterclockwise
	delay(distanceDelay);                               // ...for 2 seconds
}

void stop() //Stop function, makes the robot stop driving (if the servo's are properly set)
{
	servoLeft.writeMicroseconds(1500);         // Pin 13 stay still
	servoRight.writeMicroseconds(1500);        // Pin 12 stay still
}


int readIRMid() //reads Middle IR. Return unit to be determined
{

}

int readIRGrab()    //Reads IR Grabber. Return unit to be determined
{		

}

int readLightSense()   //Reads Light Sensor. Return unit to be determined
{	

}

int readirlb() //reads ir left bottom. return unit to be determined
{

}

int readirrb() //reads ir right bottom. return unit to be determined
{

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
	servoUltra.write(angle);
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

	Serial.print(inches);
	Serial.print("in, ");
	Serial.print(cm);
	Serial.print("cm");
	Serial.println();

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
