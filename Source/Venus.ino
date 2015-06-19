#include "Arduino.h"
#include <math.h>
#include <Servo.h>
#include "softwaredrivers.h"
#include "pathlogic.h"
#include "location.h"

// Toggle to enable/disable serial output and debugging
// NONE	OF THE SERIAL CODES WILL THUS BE COMPILED
// REDUCING FILESIZES AND SRAM USAGE
#define DEBUG true			
#define Serial if(DEBUG)Serial


// NOTE
// Everything which has been given in degrees, uses a rotated polar system as reference:
//
//                                0
//
//                              o ^ o 
//                           o    |    o  
//                      +90 o     |     o -90
//                          o     |     o 
//                           o    |    o 
//                              o   o
//
//                              +/-180
//
// It is not efficient to turn more than 180 degrees



// Things that must be included in other parts:
// - Set latestBaseIndex to the the currentIndex when restarting from the base
// - Use ShortestDistance to get back on track when driving around a cliff
// - New waypoints needs to be checked in the matrix

// ----------------------------------------------------------
// VARIABLE DECLARATION
// ----------------------------------------------------------

#define PATH_ENTRIES		100				// Maximum number of allowed paths
#define SAMPLES				37				// Number of samples to take for the top-US sensor
#define PI					3.14159265359	// Obviously
#define USSERVO_OFFSET		95				// Offset in the data for the top servo
#define SAFE_DISTANCE		17				// Offset distance due placement of top/bottom US sensor (stopping distance)
#define SAFE_ROCK_DISTANCE	10				// Offset from where the robot must stop to get the rock within grabber range
#define ROCK_RANGE			10				// Range in which the gripper is able to grab the rock
#define INTEREST_THRESHOLD  3				// When the number of visits are higher than the set number, the locations are not defined as interesting and will be ignored. Other locations will thus be prioritised.
#define DODGE_ANGLE			-90				// Angle in which the robot should turn for a cliff
#define DODGE_DISTANCE		15				// Distance the robot should drive each dodge move
#define BOTTOM_US_SENSOR	true			// Turn the sensor on or off

enum crash {
	NONE,
	US_TOP,
	US_DOWN,
	IR_DOWN,
	ROCK
} crashCause;


// current ID for the path array
unsigned int currentPathID = 0;
// Ultrasonic Sensor input array
path usData[SAMPLES] = { NULL };
byte usDown = 0;

// Crash handling variable
bool crashed = false;

// only for debugging purposes
bool debugLoop = false;
int loopCounter = 0;

// Path array
path paths[PATH_ENTRIES] = { NULL };
unsigned int latestBaseIndex = 0;

// Dodge counter
unsigned int startDodge = 0;
unsigned int dodgeCounter = 0;

// State variables
bool grabberOpen = false;
bool holdsRock = false;
bool foundRock = false;
bool bottomCrashDetection = true;
// tracking variables for forbidden moves
int forbiddenAngle = NULL;


// ----------------------------------------------------------
// PROTOTYPES
// ----------------------------------------------------------
void initiateDrive();
bool setPath(path newPath);
bool removePath(unsigned int pathID);
void changePath(unsigned int pathID, unsigned int distance, int angle, byte mapX, byte mapY);
void reversePath();
void scanSurroundings();
path shortestPath(unsigned int from, unsigned int to);
path recoverPath(unsigned int startDodge, unsigned int endDodge, path destination, unsigned int distanceDriven);
int toCartesian(path pCoordinate, byte index, bool useX);
path toPolar(byte x, byte y);
path getAbsoluteCoordinate();
bool One();

void dodge(unsigned int distance, int angle);
path dodgeCliff(unsigned int drivenDistance);
// ----------------------------------------------------------
// FUNCTIONS
// ----------------------------------------------------------

// Initiating Arduino hardware and serial communications
void setup()
{
	// Initiate serial communications
	Serial.begin(9600);
	
	// Initiate the software part
	startSetup();

	// Set the servo's to their initial positions
	grabberOpen = true;
	openGrabber();
	Serial.println("startsetup");
	calibratespeedFixedDistance(90);//Has to be done at the beginning otherwise the robot cant turn accuratly. something still has to be implemented to still be able to get out of the calibration when one of the encoders doesn't work
	calibrateWhiteIR(); //after movement calibration white value is calculated
	stop();

	// Set the rotating sensor to its center
	readUltraTop(USSERVO_OFFSET);
	
	delay(1000);
}

// Main program loop
void loop()
{
	// Start the strategy
	initiateDrive();
}

// Routine for the obstacle functions and things that needs to be handled
// during the driving procedure
bool checkObstacles()
{
	
	// Disable crash detection when reversing path
	if (bottomCrashDetection)
	{
		
		// Simple (placeholder) collission detection
		if (!IRdown())
		{
			crashCause = IR_DOWN;
			return true;
		}

		if (!One())
		{
			crashCause = US_TOP;
			return true;
		}

		if (readUltraBot() < SAFE_ROCK_DISTANCE && BOTTOM_US_SENSOR)
		{
			Serial.println("Block found");
			crashCause = ROCK;
			return true;
		}
	}


	// Return true when everything is ok
	return false;
}

// Start from lab
void initiateDrive()
{
	// Create our working path
	path newPath;
	
	// Start scanning with top US-sensor
	scanSurroundings();

	// Set a new path to the closest detected object.
	// (set the last parameter to false to gain the max value)
	// Must be extended using the padding matrix to determine
	// whether we've already been there.
	newPath = getClosestPath(usData, SAMPLES, true);

	// Save the distance for later usage
	unsigned int distance = newPath.distance;
	newPath.distance = 0;

	// Add path to array without any distance
	setPath(newPath);

	// Rotate to the direction were we've seen something interesting
	drive(0, newPath.angle);

	// Before driving a certain distance, we want to know where we're going for
	// By using the fact that the ultrasonic sensors have a height difference, we
	// can check whether we see something small or large. 
	// When seeing a small object, the top sensor will register a larger value than the bottom.
	// When seeing a large object, both sensors will register the same distance.

	// Thus first get data from the top sensor
	distance = readUltraTop(USSERVO_OFFSET);
	Serial.print(readUltraBot());
	Serial.print(" vs ");
	Serial.println(distance);

	// Check if there is a difference
	if ((readUltraBot() < distance - 1) && BOTTOM_US_SENSOR)
	{
		// Found rock
		Serial.println("Rock found");
		foundRock = true;

		// Set the new data into the path
		newPath.angle = 0;
		newPath.distance = readUltraBot();

	}
	else {
		// Set the new data into the path
		newPath.angle = 0;
		newPath.distance = distance;
	}
	
	if (newPath.distance > SAFE_DISTANCE) {
		// SAFE_DISTANCE ensures that the robot stops close to the target.
		// E.g. the rock will be in the grappers when the robot stops.
		newPath.distance -= SAFE_DISTANCE;
	} else {
		// This case shouldn't happen. It means that the top sensor measured
		// a distance that is smaller than the bottom sensor. This could be an
		// indication of a bad measurement or a blinded top sensor.
		newPath.distance = 0;
		newPath.angle = 180;
	}

	// Save the path
	setPath(newPath);
	
	// Calculate the coordinates from the given array
	path temp = getAbsoluteCoordinate();
	
	// Now we have set the path, we want to check wheter it is a useful direction to go to by using the map.

	// First convert the gathered absolute coordinates to map compatible coordinates. We'll need them a lot
	byte x = toMapCoordinate(temp.mapX);
	byte y = toMapCoordinate(temp.mapY);

	// Because we haven't driven to our objective, we will not add visits yet. 

	// First check if we have already been there
	if (getVisits(x, y) <= INTEREST_THRESHOLD || hasObstacle(x, y) == true)
	{
		// Interesting enough to look further

		// Check if there is a rock registered or if we've found a rock
		if (!hasRock(x, y) && foundRock)
			// Add the rock
			addRock(x, y);	
		// Check if there is an obstacle registered
		else if (!hasObstacle(x, y) && !foundRock)
			// Add the obstacle
			setObstacle(x, y);
	}
	else
	{
		// The location is visited too much, now we need another location
		byte targetX, targetY;
		
		// Remove the latest path (only distance) to make place for the new one
		removePath(currentPathID - 1);

		// Check whether the map has a suggested location
		if (getSuggestion(x, y, &targetX, &targetY))
		{			
			// Convert the coordinates to a polar form to make them usable as a waypoint
			newPath = toPolar(targetX, targetY);
		}
		else 
		{
			// There is nothing interesting reported, now need to drive just somewhere
			newPath.angle = 180;
			newPath.distance = 0;
		}

		// And set the new path
		setPath(newPath);

	}

	Serial.print("Waypoint: ");
	Serial.print(newPath.distance);
	Serial.print(" - ");
	Serial.print(newPath.angle);
	Serial.print(" at: (");
	Serial.print(temp.mapX);
	Serial.print(", ");
	Serial.print(temp.mapY);
	Serial.println(")");

	// We'd like to know if the full path has been driven, if not, the last waypoint needs to be changed
	DistanceType drivenDistance = drive(newPath.distance, newPath.angle);
	delay(1000);

	// Do some waypoint management

	// Merge the two waypoints
	newPath.angle = paths[currentPathID - 2].angle;
	newPath.distance = paths[currentPathID - 1].distance;

	// Remove the obsolete ones
	removePath(currentPathID - 1);
	removePath(currentPathID - 1);

	// And add the merged one again
	setPath(newPath);

	// If the driven distance appears to be less than we wanted
	if (drivenDistance < newPath.distance)
	{
		// In every case the current path needs adjustment, because the robot stopped when the distance is less than ordered
		// Note that the coordinates are adjusted automatically by the function
		changePath(currentPathID - 1, drivenDistance, NULL, NULL, NULL);
		
		// Then do some map administration

		// Get the cause of crash
		switch (crashCause)
		{
		case NONE:
			// ---
			break;
		case US_TOP:
			Serial.println("dodge");
			//dodge(0, -90);
			crashCause = NONE;
			break;
		case US_DOWN:
			// ---
			break;
		case IR_DOWN:
			//dodgeCliff(drivenDistance);
			crashCause = NONE;
			break;

		case ROCK:
			// Grab the rock
			foundRock = true;
			if (grabberOpen){
				closeGrabber();
				calibratespeedFixedDistance(100);
				calibrateWhiteIR();
				grabberOpen = false;
			}

			delay(1000);
			// Return to the base
			reversePath();
			// And drop it right there
			// TODO: the IR sensors need to be implemented here
			// TODO: Backup navigation to the base must also be implemented
			delay(1000);
			grabberOpen = true;
			openGrabber();
			
			for (int i = 0; i < currentPathID; i++)
			{
				paths[i].distance = NULL;
				paths[i].angle = NULL;
				paths[i].mapX = NULL;
				paths[i].mapY = NULL;
			}
			currentPathID = 0;

			drive(-20, 180);

			foundRock = false;
			crashCause = NONE;
			break;
		}


	}
	// Hasn't he found the rock yet?
	if (foundRock)
	{
		// The rock has not been found
		searchRock();

		// Check whether the rock has been found
		if (readUltraBot() < SAFE_ROCK_DISTANCE)
		{
			// Grab the rock
			grabberOpen = false;
			closeGrabber();
			calibratespeedFixedDistance(100);
			calibrateWhiteIR();
			delay(1000);
			// Return to the base
			reversePath();
			// And drop it right there
			// TODO: the IR sensors need to be implemented here
			// TODO: Backup navigation to the base must also be implemented
			delay(1000);
			grabberOpen = true;
			openGrabber();

			for (int i = 0; i < currentPathID; i++)
			{
				paths[i].distance = NULL;
				paths[i].angle = NULL;
				paths[i].mapX = NULL;
				paths[i].mapY = NULL;
			}
			currentPathID = 0;

			drive(-20, 180);

			// Prevent reboot due power constraints
			delay(10000);
		}
		else {
			// Nothing found, or rock already returned
			foundRock = false;
		}
	}
	// Reset the crash
	crashed = false;
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
	paths[currentPathID].mapX = toCartesian(paths[currentPathID], currentPathID, true);
	paths[currentPathID].mapY = toCartesian(paths[currentPathID], currentPathID, false);

	// Raise the array   counter
	currentPathID += 1;

	return true;
}

// Removes indexed path from the array, and resolving empty slots if necessary
bool removePath(unsigned int pathID)
{
	if (pathID == currentPathID - 1)
	{
		// We want to remove the last item in the list
		--currentPathID;
		paths[currentPathID].distance = 0;
		paths[currentPathID].angle = 0;
	}
	else {
		// We want to delete a path somewhere in between
		// The array must then be corrected to avoid errors

		// Safety check if there is an overflow going to occur
		if (currentPathID >= PATH_ENTRIES)
			return false;

		// Everything is safe, move all items.
		for (unsigned int i = pathID; i < currentPathID; ++i)
		{
			paths[i] = paths[i + 1];
		}

		// Empty the last slot
		paths[currentPathID].distance = 0;
		paths[currentPathID].angle = 0;
		
		// And substract one from the arraycounter for the next addition
		--currentPathID;
	}

	// Everything went as planned
	return true;
}

// Change a path using the override distance or angle. Default set to 0
// Can be used to change paths in case of a obstacle
void changePath(unsigned int pathID, unsigned int distance = NULL, int angle = NULL, byte mapX = NULL, byte mapY = NULL)
{
	// TODO(rob): The code below assumes that both of the following are true:
	// - pathID < PATH_ENTRIES
	// - currentPathID < PATH_ENTRIES
	// VERIFY that this is really true.

	// Replace the data if it has been changed
	if (distance != NULL) {
		paths[pathID].distance = distance;

		//Re-calculate the relative coordinates
		paths[currentPathID].mapX = toCartesian(paths[currentPathID], currentPathID, true);
		paths[currentPathID].mapY = toCartesian(paths[currentPathID], currentPathID, false);
	}

	if (angle != NULL) {
		paths[pathID].angle = angle;

		//Re-calculate the relative coordinates
		paths[currentPathID].mapX = toCartesian(paths[currentPathID], currentPathID, true);
		paths[currentPathID].mapY = toCartesian(paths[currentPathID], currentPathID, false);
	}

	if (mapX != NULL)
		paths[pathID].mapX = mapX;

	if (mapY != NULL)
		paths[pathID].mapY = mapY;
}

// Return to the lab by reversing the path array
void reversePath()
{
	bottomCrashDetection = false;
	// Some serial checkings...
	Serial.println("Returning to base: ");


	delay(2000);

	// Revert path, i to 0 to stop at the point before the base
	for (int i = currentPathID - 1; i >= 0; --i)
	{
		Serial.print(i);
		Serial.print(" - ");

		if (i == currentPathID - 1) {
			// Turn around and drive the set distance
			Serial.print(paths[i].distance);
			Serial.print(" - ");
			Serial.println(180);

			drive(paths[i].distance, 180);

			// Give it time to turn around
			delay(1000);
		}
		else if (i == 0) {
			Serial.print(paths[i].distance);
			Serial.print(" - ");
			Serial.println(-paths[i + 1].angle);
			drive(paths[i].distance, -paths[i+1].angle);
		}
		else {
			Serial.print(paths[i].distance);
			Serial.print(" - ");
			Serial.println(-paths[i+1].angle);
			drive(paths[i].distance, -paths[i + 1].angle);
		}

		delay(100);
	}
	Serial.print("-1");
	Serial.print(" - ");
	Serial.print(0);
	Serial.print(" - ");
	Serial.println(-paths[0].angle);
	drive(0, -paths[0].angle);

	Serial.println("Done!");

	delay(100);

	bottomCrashDetection = true;
}

// Compute desired angles for the top US-sensor and store the measured data so that it can be used by other functions
void scanSurroundings()
{	
	foundRock = false;

	// Set the sensor to the right
	readUltraTop(-90 + USSERVO_OFFSET);

	// Give enough time to turn before continuing
	delay(100);

	// Calculate the desired angles based on the number of samples
	int angleStep = 0;
	int angle = -90;

	// Check whether the wanted number of samples is legit and calculate the steps required
	if ((180 % (SAMPLES - 1)) != 0)
		angleStep = 30;
	else
		angleStep = 180 / (SAMPLES - 1);
	
	// gather the samples
	for (int i = 0; i < SAMPLES; ++i)
	{
		usData[i].angle = angle;
		if (angle == USSERVO_OFFSET && BOTTOM_US_SENSOR)
		{
			usDown = readUltraBot() - SAFE_ROCK_DISTANCE;
		}

		int distance = readUltraTop(angle + 90);

		// Make objects which are too close uninteresting by setting the distance to max
		if (distance < SAFE_DISTANCE) {
			//distance = 500; // set distance out of the scope
		}
		else {
			// Substract the collision distance to prevent riding into objects
			distance -= SAFE_DISTANCE;
		}

		usData[i].distance = distance;

		if (usDown < (distance + SAFE_DISTANCE) && angle == USSERVO_OFFSET && BOTTOM_US_SENSOR)
			foundRock = true;

		//usData[i].distance = random(0, 300);	// remove this line when there is an actual input

		// Calculate the next measuring direction
		angle = -90 + (i + 1)*angleStep;
	}
	delay(200);
	// look straight ahead
	readUltraTop(USSERVO_OFFSET); 
}

// Convert waypoint to coordinates
int toCartesian(path pCoordinate, byte index, bool useX)
{
	unsigned int distance = pCoordinate.distance;
	int angle = 90;

	// Get the absolute angle 
	for (int i = 0; i < index + 1; ++i)
		// TODO(rob): Check whether index is AT MOST 'PATH_ENTRIES - 1'.
		angle += paths[i].angle;

	// Convert polar to coordinates
	if (useX)
		return round(distance*cos(angle * PI / 180));
	else 
		return round(distance*sin(angle * PI / 180));

	return 0;
}

// Convert coordinates to waypoint (starting from the last waypoint
path toPolar(byte x, byte y)
{
	// Set the needed variables
	DistanceType diffX = 0, diffY = 0;

	for (int i = 0; i < currentPathID; i++)
	{
		diffX = paths[i].mapX;
		diffY = paths[i].mapY;
	}

	path newPath;

	// Determine the offset seen from the current position
	diffX += x;
	diffY += y;

	// Calculate the waypoint
	newPath.distance = round(sqrt(pow(diffX, 2) + pow(diffY, 2)));
	newPath.angle = round(atan(diffX / diffY) * (180 / PI));
	newPath.mapX = x;
	newPath.mapY = y;

	// Return the path
	return newPath;
}

// When adding a new path, first add the path using setPath, then call this function to add the coordinates to the array-entry of the path
// This to improve efficiency when doing map operations
path getAbsoluteCoordinate()
{
	int pathX = 0, pathY = 0;
	// Add the current known coordinates
	for (int i = 0; i < currentPathID - 1; ++i)
	{
		pathX += paths[i].mapX;
		pathY += paths[i].mapY;
	}

	// Add the known coordinates of the last point
	pathX += toCartesian(paths[currentPathID - 1], (currentPathID - 1), true);
	pathY += toCartesian(paths[currentPathID - 1], (currentPathID - 1), false);

	path temp;

	temp.mapX = pathX;
	temp.mapY = pathY;
	temp.angle = NULL;
	temp.distance = NULL;


	// return the path to be able to use the coordinates
	return temp;
}

bool One(){
	if (readUltraTop(USSERVO_OFFSET) < SAFE_DISTANCE){
		Serial.println("Stop");
		stop();
		return false;
	}

	return true;
}

bool IRdown() {
	return (CliffSensing());
}

// Function to compress actions for making a dodge
void dodge(unsigned int distance, int angle)
{
	// We receive the things a normal drive function would receive
	// Difference is that we need to keep track of what is happening
	// to be able to recover from our moves send to this function
	// and to keep the waypoint array organized
	path dodge;
	dodge.distance = distance;
	dodge.angle = angle;

	// Set the angle we've just been looking for
	//forbiddenAngle = -angle;

	// Add the path to the waypoint array
	setPath(dodge);

	// Execute the movement
	drive(dodge.distance, dodge.angle);

}

// When driving into a cliff, this function will try to dodge while remaining on course.
path dodgeCliff(unsigned int distanceDriven)
{
	// First set the first move
	path dodge, destination, temp, absolute;
	dodge.angle = DODGE_ANGLE;
	dodge.distance = DODGE_DISTANCE;

	// Remember the place we want to go to
	destination = paths[currentPathID - 1];

	// Assume we have the driven distance until the stop
	// Then calculate the driven part of the route 
	temp.distance = distanceDriven;

	// Calculate the remaining distance we should have driven
	int remainingDistance = destination.distance - temp.distance;

	// Remove it from the array
	removePath(currentPathID - 1);
	// Add the point again
	destination.distance = distanceDriven;
	setPath(destination);

	absolute = getAbsoluteCoordinate();

	// Saving the crashed location into the map
	setObstacle(toMapCoordinate(absolute.mapX), toMapCoordinate(absolute.mapY));
	addVisit(toMapCoordinate(absolute.mapX), toMapCoordinate(absolute.mapY));

	// Set the movement in the array
	setPath(dodge);

	// Get the relative coordinates of the last waypoint
	dodge.mapX = paths[currentPathID - 1].mapX;
	dodge.mapY = paths[currentPathID - 1].mapY;

	// Drive the dodge 
	DistanceType dodgeDriven = drive(dodge.distance, dodge.angle);

	// The angle of the new vector is determined by the x-offset
	temp.angle = 180 - round(atan((float)remainingDistance / (float)dodge.mapX) * (float)(180 / PI));

	// Then recalculate the distance to the endpoint
	temp.distance = round(sqrt(pow(dodge.mapX, 2) + pow(remainingDistance, 2)));

	// Set the newly calculated path
	setPath(temp);

	// And return the new path, so that it can be used to drive to
	return temp;
}

// We think we are close to a rock, but we haven't seen one between the grabbers. 
// Sweep around with the wheels to find the rock	
void searchRock()
{
	path grabRock;

	// Check if the rock isn't already in front of the robot
	unsigned int distance = readUltraBot();

	// First scan straight forward 
	if (distance < SAFE_DISTANCE)
	{
		// If something is within range, set a new waypoint
		grabRock.distance = distance;
		grabRock.angle = 0;

		setPath(grabRock);

		// And drive to it (no feedback (yet))
		drive(grabRock.distance, grabRock.angle);

		// Function done, initiateDrive does the grabbing part
		return;
	}
	else {
		int i = 0;

		// Run until the thing is not within range
		while (readUltraBot() > SAFE_DISTANCE)
		{
			if (i < 9)
			{
				drive(0, -10);
			}
			else if (i == 9)
			{
				drive(0, 90);
			}
			else if (i > 9)
			{
				drive(0, 10);
			}

			delay(200);

			// Check if the rock is in sight and whether it is a block
			if (readUltraBot() < readUltraTop(USSERVO_OFFSET) - 1)
			{
				// And set the data for the next waypoint
				grabRock.distance = distance;
				grabRock.angle = 0;

				setPath(grabRock);

				// Drive to it (no feedback (yet))
				drive(grabRock.distance, grabRock.angle);

				// Let the initiateDrive function solve the rest (grabbing and shit)
				return;
			}
		}

	}


}
