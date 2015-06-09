#pragma once
typedef unsigned char byte;

// NOTE: Using byte everywhere because we assume that the map cannot have
// more than 255 x 255 entries (it wouldn't fit in the memory).

// Maximum number of rocks that can be remembered by this module.
// Must be a number between and including 0 and 255.
#define MAX_ROCK_COUNT 20

// Horizontal and vertical size of the map.
// If the value is too small, then most of the measurements will not be saved.
// If the value is too high, then the map won't fit in memory.
// Must be a number between and including 0 and 255.
#define VENUS_MAP_SIZE 35

// Number of ints per square. See toMapCoordinate / fromMapCoordinate.
// If we assume that the field has a size of CxC ints (cm?), then
// using (1 + C / VENUS_MAP_SIZE) should be ok.
#define INTS_PER_SQUARE (400 / VENUS_MAP_SIZE)

// Use setRock to mark the location of the rock.
// Duplicate entries are ignored because we cannot reasonably distinguish
void addRock(byte x, byte y);
bool hasRock(byte x, byte y);
void removeRock(byte x, byte y);


// Mark the position as visited.
void addVisit(byte x, byte y);
// Get the number of visits at a given spot.
byte getVisits(byte x, byte y);

void setObstacle(byte x, byte y);
bool hasObstacle(byte x, byte y);

// Retrieve a suggestion for a new goal.
// Call this function if you are clueless and need a new travel objective.
// Returns true if there is a travel objective was found.
// When such an objective was found, targetX and targetY will be set to
// the X and Y coordinates of the location grid.
// TODO(rob): This method has not been implemented yet.
bool getSuggestion(byte x, byte y, byte* targetX, byte* targetY);

// Convert between an int and a byte.
// Assumptions:
// - int is in range [-400,400]
//   -400 if the robot starts at the left (range -400 to 0),
//   +400 if the robot starts at the right (range 0 to +400).
// - no efforts is spent on storing ints outside this range in the map.
byte toMapCoordinate(int position);
// Bytes always fit in an int, so currently an int is returned.
int fromMapCoordinate(byte coordinate);

#ifdef ONLY_FOR_TESTING_LOCATION_H
void resetMap();
extern const byte centerXY;
// minX, maxX, minY and maxY are VALID indexes in the map.
extern byte minX;
extern byte maxX;
extern byte minY;
extern byte maxY;
bool fitsInMap(byte* x, byte* y, bool forceFit);
#endif
