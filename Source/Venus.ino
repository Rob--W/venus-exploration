#include <Servo.h>
#include "constants.h"
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
	Serial.println(minValue(usData, SAMPLES, true));
	Serial.println(usData[minValue(usData, SAMPLES, true)].distance);


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

		Serial.print(i);
		Serial.print(" - ");
		Serial.print(usData[i].angle);
		Serial.print(" - ");
		Serial.println(usData[i].distance);

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