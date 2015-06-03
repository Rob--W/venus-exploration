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
#define VENUS_MAP_SIZE 40

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
// NOTE: The given methods have no meaningful implemented yet.
// TODO(rob): Add a meaningful conversion? E.g. int = cm, byte = map coordinate.
// Currently, toMapCoordinate just truncates the value, i.e. if the value is
// larger than 255, then 255 is returned.
byte toMapCoordinate(int position);
// Bytes always fit in an int, so currently an int is returned.
int fromMapCoordinate(byte coordinate);
