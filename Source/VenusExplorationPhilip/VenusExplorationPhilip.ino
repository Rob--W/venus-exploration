/*
 Name:		VenusExplorationPhilip.ino
 Created:	5/6/2015 5:03:38 PM
 Author:	Philip Spannring
*/


#include <Servo.h>
#include "softwaredrivers.h" 
//#include "ServoControl.h" 

// the setup function runs once when you press reset or power the board
void setup() {
	startSetup();

}



// the loop function runs over and over again until power down or reset
void loop() {
	stop();
	openGrabber();
	delay(2000);
	closeGrabber();
	delay(2000);
	//drive(0, 0);

}
