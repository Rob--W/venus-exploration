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
	Serial.begin(9600);
}



// the loop function runs over and over again until power down or reset
void loop() {
	drive(200, -45);
	delay(5000);
}
