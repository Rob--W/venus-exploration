#include "Arduino.h"
#include <math.h>
#include <Servo.h>

// Toggle to enable/disable serial output and debugging
// NONE	OF THE SERIAL CODES WILL THUS BE COMPILED
// REDUCING FILESIZES AND SRAM USAGE
#define DEBUG true				
#define Serial if(DEBUG)Serial

// NOTE
// Everything which has been given in degrees, uses a rotated polar system as reference:
//
//								  0
//							
//								o ^ o 
//							 o    |    o  
//						+90	o     |     o -90
//							o     |     o 
//							 o    |    o 
//							    o   o
//							
//								+/-180
//
// It is not efficient to turn more than 180 degrees


// ----------------------------------------------------------
// VARIABLE DECLARATION
// ----------------------------------------------------------

#define PATH_ENTRIES	100				// Maximum number of allowed paths
#define SAMPLES			7				// Number of samples to take for the top-US sensor
#define PI				3.14159265359	// Obviously

// Path struct holds basic information about driven paths
struct path
{
	// unsigned to prevent negative numbers
	unsigned int distance;
	// int due SRAM limitations, otherwise consuming 32*PATH_ENTRIES bytes
	int angle;
};

struct position
{
	int x;
	int y;
};

// current ID for the path array
unsigned int currentPathID = 0;
// Ultrasonic Sensor input array
path usData[SAMPLES] = { NULL };

// only for debugging purposes
bool debugLoop = false;
int loopCounter = 0;

// Path array
path paths[PATH_ENTRIES] = { NULL };

// Create servo instances using the Servo class
Servo venusLeft;
Servo venusRight;
Servo servoUltra;
Servo servoGrabber;

// Pin constants
const int leftservo = 12;
const int rightservo = 13;
const int ultraservo = 11;
const int grabberservo = 10;
const int leftencoder = 7;
const int rightencoder = 8;

// ----------------------------------------------------------
// ADDITIONAL INCLUDES
// ----------------------------------------------------------
#include "Motioncontrol.h"


// ----------------------------------------------------------
// PROTOTYPES
// ----------------------------------------------------------
void initiateDrive();
bool setPath(path newPath);
void reversePath();
void scanSurroundings();
int minValue(path arrayData[], unsigned int arrayLength, bool min);
int minValue(int arrayData[], unsigned int arrayLength, bool min);
path shortestPath(unsigned int from, unsigned int to);

// ----------------------------------------------------------
// PLACEHOLDER FUNCTIONS (to be replaced/filled in later on)
// ----------------------------------------------------------
void drive(unsigned int distance, int angle);
bool labLightVisible();
unsigned int usTop(int angle);


// ----------------------------------------------------------
// FUNCTIONS
// ----------------------------------------------------------

// Initiating Arduino hardware and serial communications
void setup()
{
	// Initiate serial communications
	Serial.begin(9600);

	// Attach servo's
	venusLeft.attach(leftservo,1300,1700);
    venusRight.attach(rightservo,1300,1700);
    servoUltra.attach(ultraservo,540,2400);
    servoGrabber.attach(grabberservo,540,2400);
    pinMode(leftencoder, INPUT);
	
}

// Main program loop
void loop()
{
	// Uncomment only when you want to test the path thingy
	/*
	initiateDrive();

	while (debugLoop){};

	if (loopCounter > 5)
	{
		debugLoop = true;
		Serial.println("Paths array");
		for (int i = 0; i < 6; ++i)
		{
			Serial.print(paths[i].angle);
			Serial.print(" - ");
			Serial.println(paths[i].distance);
		}
		Serial.println("-------");

		path shortest = shortestPath(4, 2);

		Serial.print(shortest.angle);
		Serial.print(" - ");
		Serial.println(shortest.distance);
	}
	else
		++loopCounter;
	*/
}

// Start from lab
void initiateDrive()
{
	path newPath;

	// We're standing on the lab, not knowing in which direction
	if (labLightVisible() == true)
		drive(0, 180);
	
	// Gain information using the top US-sensor
	scanSurroundings();
	// Get the interesting information from the acquired data
	unsigned int minID = minValue(usData, SAMPLES, true);

	// Set a new path to the closest detected object 
	// Must be extended using the padding matrix to determine
	// whether we've already been there.
	newPath = usData[minID];

	// Add path to array
	setPath(newPath);

	// Drive to it
	drive(newPath.distance, newPath.angle);
}

// Add a new path to the array for later reference
bool setPath(path newPath)
{
	// Check whether the array isn't full
	if (currentPathID >= PATH_ENTRIES)
		return false;

	// Store in array
	paths[currentPathID].distance = newPath.distance;
	paths[currentPathID].angle = newPath.angle;

	currentPathID += 1;

	return true;
}

// Return to the lab by reversing the path array
// NOT TESTED AT ALL
void reversePath()
{
	// omdraaien
	drive(0, 180);

	// Revert path, i to 0 to stop at the point before the base
	for (int i = currentPathID; i > 0; --i)
	{
		drive(paths[i].distance, -1*paths[i].angle);
	}
	// Look in the direction of the base (assumption)
	drive(0, -1*paths[0].angle); 

	// search base with light sensor (verify assumption)
	if(labLightVisible() == true)
	{
		drive(paths[0].distance, 0);
	} 
	else {
		// do something to find the lab back
	}
	
}

// Compute desired angles for the top US-sensor and store the measured data so that it can be used by other functions
void scanSurroundings()
{
	
	// Calculate the desired angles based on the number of samples
	int angleStep = 0;
	int angle = -90;

	// Check whether the wanted number of samples is legit and calculate the steps required
	if (angleStep = (180 % (SAMPLES - 1)) != 0)
		angleStep = 30;
	else
		angleStep = 180 / (SAMPLES - 1);
	
	// gather the samples
	for (int i = 0; i < SAMPLES; ++i)
	{
		usData[i].angle = angle;
		//usData[i].distance = usTop(angle);
		usData[i].distance = random(0, 300);	// remove this line when there is an actual input
		// Calculate the next measuring direction
		angle = -90 + (i + 1)*angleStep;
	}

}

// Determine the lowest (or highest, set min to false) distance value of a path array (returns the array index)
int minValue(path arrayData[], unsigned int arrayLength, bool min = true)
{
	path temp = arrayData[0];
	int minID = 0;

	// Run through the entire array
	for (int i = 0; i < arrayLength; ++i)
	{
		// Check whether we want the min of max value
		if (min)
		{
			// Check whether there is a value lower that the previously lowest value.
			if (arrayData[i].distance < temp.distance)
			{
				temp = arrayData[i];
				minID = i;
			}
		}
		else
		{
			// Check whether there is value larger than the previous largest value.
			if (arrayData[i].distance > temp.distance)
			{
				temp = arrayData[i];
				minID = i;
			}
		}

	}

	// Return the array ID of the wanted value.
	return minID;
}

// Determine the lowest (or highest, set min to false) value of an integer array (returns the array index)
int minValue(int arrayData[], unsigned int arrayLength, bool min = true)
{
	int temp = arrayData[0];
	int minID = 0;

	for (int i = 0; i < arrayLength; ++i)
	{
		if (min)
		{
			if (arrayData[i] < temp)
			{
				temp = arrayData[i];
				minID = i;
			}
		}
		else
		{
			if (arrayData[i] > temp)
			{
				temp = arrayData[i];
				minID = i;
			}
		}
	}

	return minID;
}

// Calculate the shortest path between to given points. Returned path is drive ready.
// NEEDS TO BE TESTED ON THE ROBOT
path shortestPath(unsigned int from, unsigned int to)
{
	// Working variables
	unsigned int pathDistance = 0;
	path newPath;
	float x = 0, y = 0;
	float angle = 90;				// Correction due rotated axis
	int finalAngle = 0;
	float pathX = 0, pathY = 0;
	int distance = 0;
	short int invert = 1;
	int reverse = 0;
	bool reversePath = false;
	
	// Check whether we want to reverse the path
	if (from > to)
	{
		// If so, swap the values so that we go from low to high through the array
		int temp = to;
		to = from;
		from = temp;
		reversePath = true;
		
	}

	// Then calculate the distance between the array indices
	pathDistance = to - from;

	// Repeatedly add the conversions from polar to cartesian
	for (int i = from; i - from < pathDistance; ++i)
	{
		// Calculate absolute angle
		angle += (paths[i].angle);
		distance = paths[i].distance;

		// Convert from polar to cartesian, minding the rotation of the field
		x = distance*cos(angle * PI / 180); 
		y = distance*sin(angle * PI / 180);

		// Add the coordinates 
		pathX += x;
		pathY += y;
	}
	// Debugging purposes
	/*
	Serial.println(pathX);
	Serial.println(pathY);
	*/

	// Correction when the x-values are negative
	if (pathX < 0)
	{
		invert = -1;
		reverse = 180;
	}

	// Calculate the shortest distance using Pythagoras. Rounding to prevent inaccuracy due conversion from float to int
	newPath.distance = round(sqrt(pow(pathX, 2) + pow(pathY, 2)));
	// And the angle using the  arc-tan and the corrections
	finalAngle =  round((atan(pathY / pathX))*(180 / PI)*invert - reverse);

	// Extra correction when we want to drive the path in reverse direction (need to find out why this is necessary)
	if (reversePath && finalAngle < 0)
	{
		finalAngle += 180;
	}
	else if (reversePath && finalAngle > 0)
	{
		finalAngle -= 180;
	}

	// Apply the final angle
	newPath.angle = finalAngle;

	return newPath;
}

