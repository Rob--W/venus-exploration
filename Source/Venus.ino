// ----------------------------------------------------------
// VARIABLE DECLARATION
// ----------------------------------------------------------

// Maximum number of allowed paths
#define PATH_ENTRIES	100
#define SAMPLES			7


// Path struct holds basic information about driven paths
struct path
{
	unsigned int distance;
	int angle;
};

// Path array
path paths[PATH_ENTRIES] = { 0 };
// current ID for the path array
unsigned int currentPathID = 0;
// Ultrasonic Sensor input array
path usData[SAMPLES] = { 0 };

// ----------------------------------------------------------
// PROTOTYPES
// ----------------------------------------------------------

// Prototypes
void initiateDrive();
bool setPath(path newPath);
void revertPath();

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
	// We're standing on the lab, not knowing in which direction

	/*	if (labLightVisible() == true)
	/		drive(0, 180);
	/	
	/	scanSurroundings()
	*/
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
void revertPath()
{
	// omdraaien
	// drive(0, 180);

	// Revert path, i to 0 to stop at the point before the base
	for (int i = currentPathID; i > 0; --i)
	{
		// Convert angle
		int angle = -1 * paths[i].angle;
		// drive(paths[i].distance, paths[i].angle);
	}

	// search base
}