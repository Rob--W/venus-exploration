#include "Arduino.h"
#include <math.h>
#include <Servo.h>
#include "softwaredrivers.h"
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



// Things that must be included in other parts:
// - Set latestBaseIndex to the the currentIndex when restarting from the base
// - Use ShortestDistance to get back on track when driving around a cliff
// - New waypoints needs to be checked in the matrix

// ----------------------------------------------------------
// VARIABLE DECLARATION
// ----------------------------------------------------------

#define PATH_ENTRIES	100				// Maximum number of allowed paths
#define SAMPLES			19				// Number of samples to take for the top-US sensor
#define PI				3.14159265359	// Obviously
#define MIN_DISTANCE	12

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
unsigned int latestBaseIndex = 0;



// ----------------------------------------------------------
// ADDITIONAL INCLUDES
// ----------------------------------------------------------


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
position toCartesian(path toCoordinate);
// ----------------------------------------------------------
// PLACEHOLDER FUNCTIONS (to be replaced/filled in later on)
// ----------------------------------------------------------


// ----------------------------------------------------------
// FUNCTIONS
// ----------------------------------------------------------

// Initiating Arduino hardware and serial communications
void setup()
{
	// Initiate serial communications
	Serial.begin(9600);
	
	startSetup();
	stop();
	delay(1000);
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

	initiateDrive();
}

// Start from lab
void initiateDrive()
{
	path newPath;

	// We're standing on the lab, not knowing in which direction
	//if (labLightVisible() == true)
	//	drive(0, 180);
	
	// Gain information using the top US-sensor
	if (loopCounter > 1)
		reversePath();

	scanSurroundings();
	// Get the interesting information from the acquired data
	unsigned int minID = minValue(usData, SAMPLES, true);

	// Set a new path to the closest detected object 
	// Must be extended using the padding matrix to determine
	// whether we've already been there.
	newPath = usData[minID];

	// Add path to array
	setPath(newPath);
	Serial.println(newPath.distance);
	Serial.println(newPath.angle);
	// Drive to it
	drive(newPath.distance, newPath.angle*-1);
	++loopCounter;
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
	for (int i = 0; i < currentPathID; ++i)
	{
		Serial.print(paths[i].angle);
		Serial.print(" - ");
		Serial.println(paths[i].distance);
	}
	delay(2000);
	// Revert path, i to 0 to stop at the point before the base
	for (int i = currentPathID; i < 0; --i)
	{
		if (i == currentPathID) {
			// Turn around and drive the set distance
			drive(paths[i].distance, 180);
			delay(1000);
		}
		else if (i == 0)
			drive(paths[i].distance, 0);
		else
			drive(paths[i].distance, paths[i - 1].angle*-1);

		delay(1000);
	}

	// Look in the direction of the base (assumption)
	delay(1000);
}

// Compute desired angles for the top US-sensor and store the measured data so that it can be used by other functions
void scanSurroundings()
{	
	readUltraTop(0);
	delay(100);
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
		int distance = readUltraTop(angle + 90);


		if (distance < MIN_DISTANCE) {
			distance = 500; // set distance out of the scope
		}
		else {
			distance -= MIN_DISTANCE;
		}
		Serial.print("a: ");
		Serial.print(angle);
		Serial.print(" - d: ");
		Serial.println(distance);

		usData[i].distance = distance*10;

		delay(0);
		//usData[i].distance = random(0, 300);	// remove this line when there is an actual input
		// Calculate the next measuring direction
		angle = -90 + (i + 1)*angleStep;
	}

	readUltraTop(90); // look straight ahead

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
/*
// Convert waypoint to coordinates
position toCartesian(path pCoordinate)
{
	position cPosition;
	int distance = pCoordinate.distance;
	unsigned int angle = pCoordinate.angle;

	// Convert polar to coordinates
	cPosition.x = distance*cos(angle * PI / 180);
	cPosition.y = distance*sin(angle * PI / 180);

	return cPosition;
}*/