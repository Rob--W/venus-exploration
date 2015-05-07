// ----------------------------------------------------------
// VARIABLE DECLARATION
// ----------------------------------------------------------

#define PATH_ENTRIES	100		// Maximum number of allowed paths
#define SAMPLES			7		// Number of samples to take for the top-US sensor


// Path struct holds basic information about driven paths
struct path
{
	unsigned int distance;
	int angle;
};

// Path array
path paths[PATH_ENTRIES] = { NULL };
// current ID for the path array
unsigned int currentPathID = 0;
// Ultrasonic Sensor input array
path usData[SAMPLES] = { NULL };

// ----------------------------------------------------------
// PROTOTYPES
// ----------------------------------------------------------
void initiateDrive();
bool setPath(path newPath);
void reversePath();
void scanSurroundings();
int minValue(path arrayData[], unsigned int arrayLength);
int minValue(int arrayData[], unsigned int arrayLength);

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

  /* add setup code here */
	//blablabla better works
}

// Main program loop
void loop()
{

  /* add main program code here */

}

// Start from lab
void initiateDrive()
{
	path newPath;

	// We're standing on the lab, not knowing in which direction
	if (labLightVisible() == true)
		drive(0, 180);
		
	scanSurroundings();

	newPath = paths[minValue(usData, SAMPLES + 1)];

	setPath(newPath);

	drive(newPath.distance, newPath.angle);
}

// Add a new path to the array for later reference
bool setPath(path newPath)
{
	// Check whether the array isn't full
	if (currentPathID >= PATH_ENTRIES)
		return false;

	currentPathID += 1;

	// Store in array
	paths[currentPathID].distance = newPath.distance;
	paths[currentPathID].angle = newPath.angle;

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
	
	// gather the samples
	for (int i = 0; i < SAMPLES; ++i)
	{
		usData[i].angle = angle;
		usData[i].distance = usTop(angle);
		angle += angleStep;
	}

}

// Determine the lowest distance value of a path array (returns the array index)
int minValue(path arrayData[], unsigned int arrayLength)
{
	path temp = arrayData[0];
	int minID = 0;

	for (int i = 0; i < arrayLength; ++i)
	{
		if (arrayData[i].distance < temp.distance)
		{
			temp = arrayData[i];
			minID = i;
		}
	}

	return minID;
}

// Determine the lowest value of an integer array (returns the array index)
int minValue(int arrayData[], unsigned int arrayLength)
{
	int temp = arrayData[0];
	int minID = 0;

	for (int i = 0; i < arrayLength; ++i)
	{
		if (arrayData[i] < temp)
		{
			temp = arrayData[i];
			minID = i;
		}
	}

	return minID;
}