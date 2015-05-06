#define PATH_ENTRIES  100

// Global variables
struct path
{
	unsigned int distance;
	int angle;
};

path paths[PATH_ENTRIES] = { 0 };
unsigned int currentPathID = 0;

// Prototypes
void initiateDrive();
bool setPath(path newPath);
void revertPath();


void setup()
{

  /* add setup code here */
	//blablabla better works
}

void loop()
{

  /* add main program code here */

}

void initiateDrive()
{
	// We're standing on the lab, not knowing in which direction

	/*	if (labLightVisible() == true)
	/		drive(0, 180);
	/	
	/	scanSurroundings()
	*/
}

bool setPath(path newPath)
{
	// Check whether the array isn't full
	if (currentPathID >= PATH_ENTRIES)
		return false;

	// Convert angle
	int angle = -1 * newPath.angle;

	currentPathID += 1;

	// Store in array
	paths[currentPathID].distance = newPath.distance;
	paths[currentPathID].angle = newPath.angle;

	return true;
}

void revertPath()
{
	// omdraaien
	// drive(0, 180);

	// Revert path, i to 0 to stop at the point before the base
	for (int i = currentPathID; i > 0; --i)
	{
		// drive(paths[i].distance, paths[i].angle);
	}

	// search base
}