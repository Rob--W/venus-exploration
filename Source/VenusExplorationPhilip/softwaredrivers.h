#pragma once

void startSetup(); //Content needed for the pin initialisation in function setup()

void stop();	//Stop function, makes the robot stop driving (if the servo's are properly set)

void drive(int distance, int angle);//drive function with integer parameters distance and angle in degrees

void reverse(int distance);	//Makes the robot drive backwards a certain distance

int readIRLB();		//Reads IR Left Bottom. Return unit to be determined

int readIRRB();		//Reads IR Right Bottom. Return unit to be determined

int readIRMid();	//reads Middle IR. Return unit to be determined

int readIRGrab();		//Reads IR Grabber. Return unit to be determined

int readLightSense();	//Reads Light Sensor. Return unit to be determined

int readUltraTop(int angle);		//Reads Top Ultrasonic sensor. Return unit is a distance. Input parameter is a angle; full left is angle 0 degrees full right is 180 degrees

int readUltraBot(); //Reads bottom Ultrasonic sensor. Return unit is a distance.

void openGrabber(); //Opens the grabber

void closeGrabber(); //closes the grabbr