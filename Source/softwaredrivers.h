#pragma once

#define US_BOTPIN_O 4
#define US_BOTPIN_I 5
#define mmconv				1000.0 //meter times 1000 = mm
#define IR_RB A0
#define IR_LB A1
#define motorpower	90		//define percentagepower in drive/philipsforward function
#define IRsafety 100


bool checkObstacles();
enum colour { BLACK, WHITE, GREY, OTHER };


void startSetup(); //Content needed for the pin initialisation in function setup()
/*
void turn();
*/
enum colour readIRLB();		//Reads IR Left Bottom. Return unit to be determined. has to be analog

enum colour readIRRB();		//Reads IR Right Bottom. Return unit to be determined. has to be analog


void stop();	//Stop function, makes the robot stop driving (if the servo's are properly set)

void ServoUturn(int angle);
void calibrateWhiteIR();
void calibrateBlackIR();
//left: value between 0 and 180 that drives the left servo
//right:value between 0 and 180 that drives the right servo
//0 is the turn to one direction. 90 is stop. 180 is turn to the other direction.
//n: value bigger than 0 that defines the number of pulses recieved from the digital encoder. (distance)
//void servoDrive(int left, int right, int n);	//servo control for the repetitive part of the drive functions.

//distance
// distance is the maximum distance (in cm).
// Returns the traveled distance (in cm).
int drive(int distance, int angle);
//Function to measure the speed of the wheel at a certain amount of the wheels power
void calibratespeedFixedDistance(int percentagePower);
// Move forwards at the full speed.
// distanceDelay is the approximate time needed to drive distance (in cm).
// Returns the traveled distance (in cm).
int FForward(long distanceDelay);
void FBack(long distanceDelay);
void PRight(int angleDelay);
void PLeft(int angleDelay);
void Turn180();
/*
void drive(int distance, int angle);//drive function with integer parameters distance and angle in degrees

void reverse(int distance);	//Makes the robot drive backwards a certain distance
*/
int readIRMid();	//reads Middle IR. Return unit to be determined

colour readIRGrab();		//Reads IR Grabber. Return unit to be determined

int readLightSense();	//Reads Light Sensor. Return unit to be determined

unsigned int readUltraTop(int angle);		//Reads Top Ultrasonic sensor. Return unit is a distance. Input parameter is a angle; full left is angle 0 degrees full right is 180 degrees

unsigned int readUltraBot(); //Reads bottom Ultrasonic sensor. Return unit is a distance.

void openGrabber(); //Opens the grabber

void closeGrabber(); //closes the grabbr
bool CliffSensing(); //looks for cliffs/black lines; true is no cliff; false is cliff