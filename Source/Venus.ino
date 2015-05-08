<<<<<<< HEAD
//#include "constants.h"

#include "Arduino.h"
#include <math.h>

// Toggle to enable/disable serial output and debugging
// NONE	OF THE SERIAL CODES WILL THUS BE COMPILED
// REDUCING FILESIZES AND SRAM USAGE
#define DEBUG true				
#define Serial if(DEBUG)Serial

// ----------------------------------------------------------
// VARIABLE DECLARATION
// ----------------------------------------------------------

#define PATH_ENTRIES	100		// Maximum number of allowed paths
#define SAMPLES			7		// Number of samples to take for the top-US sensor
#define PI				3.14159265359

// Path struct holds basic information about driven paths
struct path
{
	unsigned int distance;
	float angle;
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

=======
#include <Servo.h>
#include "constants.h"
#include "Motioncontrol.h"
>>>>>>> origin/master

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
	venusLeft.attach(leftservo,1300,1700);
    venusRight.attach(rightservo,1300,1700);
    servoUltra.attach(ultraservo,540,2400);
    servoGrabber.attach(grabberservo,540,2400);
    pinMode(leftencoder, INPUT);
	
}

// Main program loop
void loop()
{


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

		path shortest = shortestPath(1, 5);

		Serial.print(shortest.angle);
		Serial.print(" - ");
		Serial.println(shortest.distance);
	}
	else
		++loopCounter;
}

// Start from lab
void initiateDrive()
{
	path newPath;

	// We're standing on the lab, not knowing in which direction
	//if (labLightVisible() == true)
	//	drive(0, 180);
		
	scanSurroundings();
	unsigned int minID = minValue(usData, SAMPLES, true);
	newPath = usData[minID];

	setPath(newPath);

	//drive(newPath.distance, newPath.angle);
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
		usData[i].distance = random(0, 300);
		// Calculate the next measuring direction
		angle = -90 + (i + 1)*angleStep;
	}

}

// Determine the lowest (or highest, set min to false) distance value of a path array (returns the array index)
int minValue(path arrayData[], unsigned int arrayLength, bool min = true)
{
	path temp = arrayData[0];
	int minID = 0;

	for (int i = 0; i < arrayLength; ++i)
	{
		if (min)
		{
			if (arrayData[i].distance < temp.distance)
			{
				temp = arrayData[i];
				minID = i;
			}
		}
		else
		{
			if (arrayData[i].distance > temp.distance)
			{
				temp = arrayData[i];
				minID = i;
			}
		}

	}

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
path shortestPath(unsigned int from, unsigned int to)
{
	// Temporary variables
	unsigned int pathDistance = 0;
	float pathX = 0, pathY = 0;
	int reverse = 1;
	int start = 0;

	// Determine whether we go forward or backward in our history
	if (from > to)
	{
		reverse = -1;
		start = to;
		pathDistance = from - to;
	}
	else
	{
		start = from;
		pathDistance = to - from;
	}
	
	// Repetitive addition of the x and y projections of each path within the given range
	for (int i = 0; i < pathDistance; ++i)
	{
		float x = 0, y = 0;
		unsigned int distance = paths[i + start].distance;
		int angle = paths[i + start].angle;

		x = (distance)*cos(angle * (PI / 180));
		y = (distance)*sin(angle * (PI / 180));

		pathX += x;
		pathY += y;
	}


	// create a new path
	path newPath;

	// The arduino will never drive in an angle bigger than +/- 90 degrees, because of the top us Sensor
	// Therefore no check added for cases when the int may overflow due a bigger angle.
	
	// fill the path with the acquired data, so that it can be returned
	// Distance is calculated using Pythagoras'squares
	newPath.distance = (unsigned int)sqrt(pow(pathX, 2) + pow(pathY, 2));
	// And the angle using the arc-cosine and the propertie of history (times 1 or -1)
	newPath.angle = acos(pathX / newPath.distance)*(180 / PI) * reverse;

	return newPath;
}