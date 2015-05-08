#include "Arduino.h"

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

// Path struct holds basic information about driven paths
struct path
{
	unsigned int distance;
	int angle;
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