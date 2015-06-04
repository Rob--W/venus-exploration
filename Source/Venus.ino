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
#define SAMPLES				19				// Number of samples to take for the top-US sensor
#define PI					3.14159265359	// Obviously
#define USSERVO_OFFSET		90				// Offset in the data for the top servo
#define SAFE_DISTANCE		15				// Offset distance due placement of top/bottom US sensor (stopping distance)
#define INTEREST_THRESHOLD  3				// When the number of visits are higher than the set number, the locations are not defined as interesting and will be ignored. Other locations will thus be prioritised.

// current ID for the path array
unsigned int currentPathID = 0;
// Ultrasonic Sensor input array
path usData[SAMPLES] = { NULL };

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
void IRU();
bool IRM();
bool IRG();
void light();
void USU();
void USD();
bool One();

void dodge(unsigned int distance, int angle);

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
	// Start the strategy
	initiateDrive();

}

// Routine for the obstacle functions and things that needs to be handled
// during the driving procedure
bool checkObstacles()
{
	// Simple (placeholder) collission detection
	if (!One())
		crashed = true;				// Collission detected!

	// Return true when everything is ok
	return crashed;
}

// Start from lab
void initiateDrive()
{
	path newPath;

	// We're standing on the lab, not knowing in which direction
	//if (labLightVisible() == true)
	//	drive(0, 180);
	
	// Gain information using the top US-sensor
	if (loopCounter > 3) {
		reversePath();
		while (true){}
	}

	// Start scanning with top US-sensor
	scanSurroundings();

	// Set a new path to the closest detected object.
	// (set the last parameter to false to gain the max value)
	// Must be extended using the padding matrix to determine
	// whether we've already been there.
	newPath = getClosestPath(usData, SAMPLES, true);


	// Add path to array
	setPath(newPath);

	// Calculate the coordinates from the given array
	path temp = getAbsoluteCoordinate();

	if (getVisits(temp.mapX, temp.mapY) <= INTEREST_THRESHOLD){
		// If the map hasn't registered the object, add it to the map
		if (!hasObstacle(temp.mapX, temp.mapY))
			setObstacle(temp.mapX, temp.mapY);
		
		// Add a visit, to make the location less interesting
		addVisit(temp.mapX, temp.mapY);
	}
	else {
		// Create variables where getSuggestion can store the results in
		byte suggestedX = 0, suggestedY = 0;

		// Check whether there is a direction which could be interesting
		if (getSuggestion(temp.mapX, temp.mapY, &suggestedX, &suggestedY))
		{
			// The given location is interesting, we need to head in that location

			// TODO: calculate the given coordinates back to a vector that is drivable from the point the robots are standing now
		}
		else {
			// The given location is not interesting, fallback on some more primitive route changes

			// Thus remove the current path
			removePath(currentPathID - 1);

			// set a dodging move
			newPath.angle = 90;
			newPath.distance = 20;

			// And add to the waypoint library
			setPath(newPath);
		}
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

	// If the driven distance appears to be less than we wanted
	if (drivenDistance < newPath.distance)
	{
		// set the changed distance
		changePath(currentPathID - 1, drivenDistance, NULL, NULL, NULL);

		// then recalculate and change the coordinates
		path temp = getAbsoluteCoordinate();

		// Set the new obstacle at the position we justed stopped
		setObstacle(temp.mapX, temp.mapY);
		// And add a visit
		addVisit(temp.mapX, temp.mapY);

		Serial.print("Changed path length: ");
		Serial.println(drivenDistance);
	}


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

	// Raise the array counter
	currentPathID += 1;

	return true;
}

// Removes indexed path from the array, and resolving empty slots if necessary
bool removePath(unsigned int pathID)
{
	if (pathID = currentPathID - 1)
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

		// Empty the indexed slot
		paths[pathID].distance = 0;
		paths[pathID].angle = 0;

		path temp;

		// Everything is safe, start swapping from the removed item to the last added item
		for (int i = 0; i < currentPathID - pathID; ++i)
		{
			// pathID has just been emptied, so lets start right there
			// swappedy swap!
			temp = paths[i + pathID];
			paths[i + pathID] = paths[i + pathID + 1];
			paths[i + pathID + 1] = temp;
		}
		
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
	// Replace the data if it has been changed
	if (distance >= 0 && distance != NULL) {
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

		delay(1000);
	}
	Serial.print("-1");
	Serial.print(" - ");
	Serial.print(0);
	Serial.print(" - ");
	Serial.println(-paths[0].angle);
	drive(0, -paths[0].angle);

	Serial.println("Done!");

	delay(1000);
}

// Compute desired angles for the top US-sensor and store the measured data so that it can be used by other functions
void scanSurroundings()
{	
	// Set the sensor to the right
	readUltraTop(-90 + USSERVO_OFFSET);

	// Give enough time to turn before continuing
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

		// Make objects which are too close uninteresting by setting the distance to max
		if (distance < SAFE_DISTANCE) {
			//distance = 500; // set distance out of the scope
		}
		else {
			// Substract the collision distance to prevent riding into objects
			distance -= SAFE_DISTANCE;
		}

		Serial.print("a: ");
		Serial.print(angle);
		Serial.print(" - d: ");
		Serial.println(distance);

		usData[i].distance = distance;

		//usData[i].distance = random(0, 300);	// remove this line when there is an actual input

		// Calculate the next measuring direction
		angle = -90 + (i + 1)*angleStep;
	}
	
	// look straight ahead
	readUltraTop(90); 
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
	
	// In case of the first entry
	if (to == 0 && from == 0)
	{
		x = paths[0].distance*cos(paths[0].angle * PI / 180);
		y = paths[0].distance*sin(paths[0].angle * PI / 180);

		pathX = x;
		pathY = y;
	}
	else {
		// Repeatedly add the conversions from polar to cartesian
		for (int i = from; i - from <= pathDistance; ++i)
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
	}

	// Correction when the x-values are negative
	if (pathX < 0)
	{
		invert = -1;
		reverse = 180;
	}

	// Calculate the shortest distance using Pythagoras. Rounding to prevent inaccuracy due conversion from float to int
	newPath.distance = round(sqrt(pow(pathX, 2) + pow(pathY, 2)));
	// And the angle using the  arc-tan and the corrections
	finalAngle = round((atan(pathY / pathX))*(180 / PI)*invert - reverse);

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

// In case a cliff or another obstacle occurs, the path needs to be restored
// This calculates the offset caused by other movements to change the direction vector
// to get to the earlier defined destination
path recoverPath(unsigned int startDodge, unsigned int endDodge, path destination, unsigned int distanceDriven)
{
	// First calculate the shortest path of the offset movements to simplify the calculations
	path simplifiedDodge = shortestPath(startDodge, endDodge);
	path temp;
	// Then calculate the required changes in angle and distance to the destination using the simplified path
	
	// First determine the projections of the dodge move
	int angle = 0;
	int pathX, pathY;
	int invert = 1;

	// Remember the angle sign
	if (simplifiedDodge.angle < 0)
		invert = -1;

	// We need the other part of the angle
	angle = (invert * 90) - simplifiedDodge.angle;
	
	// Determine the cartesian projection
	pathX = round(-1 * invert * simplifiedDodge.distance * cos(angle * PI / 180)); 
	pathY = round(simplifiedDodge.distance * sin(angle * PI / 180));

	// Assume we have the driven distance until the stop
	// Then calculate the driven part of the route 
	temp.distance = distanceDriven + pathY;

	int remainingDistance = destination.distance - temp.distance;
	// The angle of the new vector is determined by the x-offset
	temp.angle = round(atan(pathX / remainingDistance) * (180 / PI));

	// Then recalculate the distance to the endpoint
	temp.distance = round(sqrt(pow(pathX, 2) + pow(remainingDistance, 2)));

	// Clean up the array and add the newly calculated paths
	for (int i = 0; i < (endDodge - startDodge); ++i)
	{
		// The removePath function resolves empty slots automatically, so we don't have to
		// pass the index to the function.
		removePath(startDodge);
	}

	// Set the newly calculated paths
	setPath(simplifiedDodge);
	setPath(temp);

	// And return the new path, so that it can be used to drive to
	return temp;
}

// Convert waypoint to coordinates
int toCartesian(path pCoordinate, byte index, bool useX)
{
	unsigned int distance = pCoordinate.distance;
	int angle = 90;

	// Get the absolute angle 
	for (int i = 0; i < index + 1; ++i)
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
	path current = paths[currentPathID - 1];
	DistanceType diffX = 0, diffY = 0;

	path newPath;

	// Determine the offset seen from the current position
	diffX = current.mapX + x;
	diffY = current.mapY + y;

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

/*UltrasoonUP*/
void USU(){
	if (readUltraTop(USSERVO_OFFSET) > 280){
		if (readUltraBot() < 280){							//we might have found a stone
			drive(readUltraBot(), 0);							// go for it wall-e
			stop();
			USU();											//make a re-check, calibration
		}
		else{												//nothing found yet
			drive(25, 0);									//we drive a little further and try it again
			USU();
		}
	}
	else if (readUltraTop(USSERVO_OFFSET) > 400 && readUltraTop(USSERVO_OFFSET) < 280){
		if (abs(readUltraBot() - readUltraTop(USSERVO_OFFSET)) < 10){
			drive(readUltraTop(USSERVO_OFFSET), 0);						//we are going on an adventure
		}
		else if ((readUltraBot() - readUltraTop(USSERVO_OFFSET)) > 10){ //There is an object closer to Top than Bot so let's explore it
			drive(readUltraTop(USSERVO_OFFSET), 0);
		}
		else{											//maybe a possible stone 0.o
			drive(readUltraBot(), 0);
		}
	}
	else{
		if (abs(readUltraBot() - readUltraTop(USSERVO_OFFSET)) > SAFE_DISTANCE){ //we are standing in front of a rock....
			stop();
			drive(10, 90);									//rotate 10 cm to the right
			USU();											//repeat le process
		}
		else{
			stop();
			//IRM();										//start stone scan procedure
		}
	}
}
/*UltrasoonDown*/
void USD(){
	if (readUltraBot() < 15){
		drive(5, 0);			//rijd er nog iets dichter naartoe
		IRM();
	}//re-scan
	else{
		USU();					//false alarm, retry
	}
}

/*IRUnder*/
void IRU(){
	int counter = 0;
	if (readIRLB() || readIRRB() < 15){
		stop();												//car stops, because end of the world or a cliff
		if (readUltraTop > 0){
			drive(readUltraTop(USSERVO_OFFSET), 0);
		}
		else{												//no new point found
			drive(5, 90);									//drive 5cm to the right
			drive(0, -90);									//drive back to the left
			drive(/*shortestpath functie*/0, 0);
		}
		if (readIRLB() || readIRRB() > 100 && readIRLB() || readIRRB() < 240){
			if (counter > 0){								//count grey stripes
				openGrabber();								//drop the stone
				counter = 0;
				//reverse(25);
			}
			else{											//i can count grey stripes
				++counter;
			}
		}
		else{
			counter = 0;
		}
	}
}
/*IRMmid*/
bool IRM(){
	if (readIRMid() == 1){
		drive(5, 0);
		stop();
		openGrabber();					//get the stone
		closeGrabber();					//we got the stone
		reversePath();					//get the stone home wall-e
	}
	else{
		stop();							//stop
		USD();							//so what did i see
	}
}

/*IRGrabber*/
bool IRG(){
	if (readIRGrab() == 0){				//We found a wall of the lab, but not the ramp
		stop();
		drive(5, 90);					// 5 cm to the right
		stop();
		drive(0, -90);					//back to the begin position
		IRG();							//retry if we found the
	}
	else{								//we found the ramp, go for it wall-e
		drive(5, 0);					//straight to the the goal	
		IRU();							//scan the ground for the grey stripes
		stop();							//stop
		IRG();							//drive further
	}
}

bool One(){
	if (readUltraTop(USSERVO_OFFSET) < SAFE_DISTANCE){
		Serial.println("Stop");
		stop();
		delay(1000);
		drive(0, 90);
		path dodge;
		dodge.distance = 0;
		dodge.angle = 90;
		setPath(dodge);
		delay(1000);
		return false;
	}

	return true;
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

	// First dodge move? Save the currentPathID
	if (startDodge == 0)
		startDodge = currentPathID;

	// Keep track of the number of moves
	++dodgeCounter;

	// Add the path to the waypoint array
	setPath(dodge);

	// Execute the movement
	drive(dodge.distance, dodge.angle);

}
