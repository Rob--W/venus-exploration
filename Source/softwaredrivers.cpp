#include <Arduino.h>
#include <Servo.h>
#include "softwaredrivers.h"


Servo venusLeft;
Servo venusRight;
Servo servoUltra;
Servo servoGrabber;

const int leftservo = 12;
const int rightservo = 13;
const int ultraservo = 11;
const int grabberservo = 10;



const int leftencoder = 7;
const int rightencoder = 8;

const int pingPin = 9;



void startSetup()
{
	venusLeft.attach(leftservo, 540, 2400);
	venusRight.attach(rightservo, 540, 2400);
	servoUltra.attach(ultraservo, 540, 2400);
	servoGrabber.attach(grabberservo, 540, 2400);

	pinMode(leftencoder, INPUT);
	pinMode(rightencoder, INPUT);
}

void turn()
{
	venusLeft.write(120);
	venusRight.write(120);
}

int readIRLB() //Reads IR Left Bottom. Return unit to be determined
{

}

int readIRRB() //Reads IR Right Bottom. Return unit to be determined
{

}

void stop() //Stop function, makes the robot stop driving (if the servo's are properly set)
{
	venusLeft.write(90);
	venusRight.write(90);
}

void servoDrive(int left, int right, int n)
{ 
	int pulse;
	int prevpulse;
	int countpulse = 0;

	prevpulse = digitalRead(rightencoder); //initial value encoder

	venusLeft.write(left);
	venusRight.write(right);

	//////////////////////////////////////////////////////////////////////////
	//																		//
	//   Continuous while loop to place scanning function in for Marijn		//
	//		vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv			//
	//////////////////////////////////////////////////////////////////////////
	while (n>countpulse)
	{
		pulse = digitalRead(rightencoder);
		if (pulse != prevpulse){
			prevpulse = pulse;
			countpulse += 1;
		}
		//400 is arbitrary between  0 and 1024. This has to be tested. 
		//The value is somewhere in between 0 and 2^8 because of a change in light and dark reception of the sensor.
		//it will never be completly dark, that is why the value will never be zero.
		/*if (readIRLB() < 400 || readIRRB() < 400)  //hole sensing code has to be added
		{
			break; //Some kind of notification has to be added that an emergency brake has been executed.
		}*/
	}
	stop();
}

void drive(int distance, int angle)  //drive function with integer parameters distance and angle in degrees
{
	//turn part
	/*angle = angle % 360;*/ //2*pirad*k=2*pirad*(k+1)
	if (angle > 0)  //turn left
	{	
		int turnn = (((angle)* 100) / 1437); //Number of pulses needed to turn. turns with both wheels
		servoDrive(100, 100, turnn);
		//int turnn = ((angle * 200) / 1437); //turns with just one wheel
		//servoDrive(90, 110, turnn);
	}
	else   //turn right
	{	
		int turnn = (((angle) *100) / 1437); //Number of pulses needed to turn. turns with both wheels
		servoDrive(80, 80, turnn);
		//int turnn = (((360-angle) * 200) / 1437); //turns with just one wheel
		//servoDrive(90, 70, turnn);
	}
	//forward movement part
	int n = round((distance*100.0) / 1317.0); //Number of pulses needed to drive distance. Distance in mm
	servoDrive(180, 0, n);
}

void reverse(int distance) //Makes the robot drive backwards a certain distance
{	
	int n = ((distance*100) / 1317); //Number of pulses needed to drive distance
	Serial.println(n);
	servoDrive(0, 180, n);
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