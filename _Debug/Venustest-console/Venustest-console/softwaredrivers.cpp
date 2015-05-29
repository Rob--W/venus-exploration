#include "softwaredrivers.h"

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

}

int drive(unsigned int distance, int angle)
{
	if (angle == 180) {
		Turn180();
	}
	else if (angle > 0) {
		// left angle
		PLeft(abs(angle) * round(555.0 / 90));
	} else if (angle < 0) {
		// right angle
		PRight(abs(angle) * round(555.0 / 90));
	} 

	return FForward(distance * round(2000.0 / 30));
}

// Full speed forward
int FForward(int distanceDelay) {
	return (distanceDelay * 30 / 2000);
}

// Turn left in place
void PLeft(int angleDelay){
                              // ...for 0.6 seconds (er stond 600)
}

// Turn right in place
void PRight(int angleDelay){
                              // ...for 0.6 seconds ( er stond 600)
}
// Full speed backward
void FBack(int distanceDelay){
                       // ...for 2 seconds
}

void Turn180() {

}

void stop() //Stop function, makes the robot stop driving (if the servo's are properly set)
{

}


int readIRMid() //reads Middle IR. Return unit to be determined
{
	return 0;
}

int readIRGrab()    //Reads IR Grabber. Return unit to be determined
{		
	return 0;
}

int readLightSense()   //Reads Light Sensor. Return unit to be determined
{	
	return 0;
}

int readirlb() //reads ir left bottom. return unit to be determined
{
	return 0;
}

int readirrb() //reads ir right bottom. return unit to be determined
{
	return 0;
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

	return 0;
}

int readUltraBot()
{
	return 0;
}
void openGrabber()
{
}
void closeGrabber()
{
}
