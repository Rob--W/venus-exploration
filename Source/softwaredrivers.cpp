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

#define leftservo	 12
#define rightservo	 13
#define ultraservo	 11
#define grabberservo 10

unsigned int DraaiCounter;
unsigned int Delay;
unsigned int Duration;

#define leftencoder	 7
#define rightencoder 8

#define pingPin		 9

void startSetup() 
{
	servoLeft.attach(leftservo);
	servoRight.attach(rightservo);
	servoUltra.attach(ultraservo, 540, 2400);
	servoGrabber.attach(grabberservo, 540, 2400);

	pinMode(leftencoder, INPUT);
	pinMode(rightencoder, INPUT);
	pinMode(US_BOTPIN_O, OUTPUT);
	pinMode(US_BOTPIN_I, INPUT);

}

int drive(unsigned int distance, int angle)
{
	if (angle == 180) {
		Turn180();
	}
	else if (angle > 0) {
		// left angle

		PLeft(abs(angle) * round(555 / 90));
	} else if (angle < 0) {
		// right angle
		PRight(abs(angle) * round(555 / 90));
	} 

	return FForward(distance * round(2000.0 / 30));

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
	return cm - 2;
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
