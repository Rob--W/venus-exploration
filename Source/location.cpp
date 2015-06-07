#include "location.h"
#include <cstring>

#if (MAX_ROCK_COUNT) & ~0xFF == 0
#error MAX_ROCK_COUNT must be a number between and including 0 and 255.
#endif

#if (VENUS_MAP_SIZE) & ~0xFF == 0
#error VENUS_MAP_SIZE must be a number between and including 0 and 255.
#endif

// Rock logic
// 

// Optimized structure for storing rock observations.
struct Position {
    byte x;
    byte y;
} rocks[MAX_ROCK_COUNT];
byte rockCount = 0;

void addRock(byte x, byte y) {
    if (rockCount > MAX_ROCK_COUNT) {
        // Ignore additional rocks.
        return;
    }
    Position& rock = rocks[rockCount];
    rock.x = x;
    rock.y = y;
    ++rockCount;
}

bool hasRock(byte x, byte y) {
    for (byte i = 0; i < rockCount; ++i) {
        Position rock = rocks[i];
        if (rock.x == x && rock.y == y) {
            return true;
        }
    }
    return false;
}

void removeRock(byte x, byte y) {
    for (byte i = 0; i < rockCount; ++i) {
        Position rock = rocks[i];
        if (rock.x == x && rock.y == y) {
            // Remove rock from list
            for (byte j = i + 1; j < rockCount; ++j) {
                rocks[j - 1] = rocks[j];
            }
            --rockCount;
            // And repeat from i again, in case
            // of duplicate rocks.
        }
    }
}

// Map logic
//

// Map for storing hill / cliff observations.
// A map entry consists of two positions.
// A position is represented by four bits, as follows:
// 0000 = unknown, never visited before
// 0001 = visited once
// 0010 = visited twice
// 0011 = visited thrice
// ... et cetera, up to seven times ...
// 1000 = obstacle, never visited.

// 4 bits are used for storing information, so each byte can contain two pieces
// of information. The values for consecutive X coordinates are combined with
// each other.
// For example, the area given by   A B C D E  0
//                                  F G H I J  1
//
//                                  0 1 2 3 4  These numbers are the coordinates
//                                             in the map.
// is represented as
// map[0][0] = AB
// map[0][1] = FG
// map[1][0] = CD
// map[1][1] = HI
// map[2][0] = E_ (where _ = 0000)
// map[2][1] = J_
#define VENUS_MAX_X ((VENUS_MAP_SIZE + 1) / 2)
#define VENUS_MAX_Y (VENUS_MAP_SIZE)
byte map[VENUS_MAX_X][VENUS_MAX_Y];

// Translate coordinates, to maximize the use of the map.
// The map's maximum size is 255x255, but due to memory constraints, the actual
// maximum size is much smaller. To make sure that the space in the map is used
// optimally, we have to remember the min and max value of the coordinates, and
// also remember the difference between the initial coordinate and the ranges.
// If x >= centerXY, then x is stored at the start of the map.
// If y < centerXY, then x is stored at the end of the map.
// Example:
// center = 7
// min = 4
// max = 9
// map size = 5
// layout of coordinates in the map: [7, 8, 9, 6, 5, 4]
#ifdef ONLY_FOR_TESTING_LOCATION_H
// All existing tests assume that 0, 1, ... can safely be stored, so we have
// to pick a "center" close to 0.
const byte centerXY = 5;
#else
const byte centerXY = 127;
#endif
// minX, maxX, minY and maxY are VALID indexes in the map.
byte minX = centerXY;
byte maxX = centerXY;
byte minY = centerXY;
byte maxY = centerXY;

#ifdef ONLY_FOR_TESTING_LOCATION_H
void resetMap() {
    minX = centerXY;
    maxX = centerXY;
    minY = centerXY;
    maxY = centerXY;
    memset(map, 0, sizeof(map));
}
#endif

// Helper function for fitsInMap. Checks whether coordinate is in the given
// range [min, max].
// Returns whether the range was changed.
byte tryToFitAtCoordinate(const byte coordinate, byte* min, byte* max) {
    if (coordinate < *min) {
        *min = coordinate;
    } else if (coordinate > *max) {
        *max = coordinate;
    } else {
        return false;
    }
    return true;
}

// Try to fit the measured data in the map.
// Input: x and y coordinates of the map
// Output: Return value is true if (x,y) can be stored in the map (in this case,
// x and y may be modified to hold valid indexes for storing data in the map).
bool fitsInMap(byte* x, byte* y, bool forceFit = false) {
    byte desiredMinX = minX;
    byte desiredMaxX = maxX;
    byte desiredMinY = minY;
    byte desiredMaxY = maxY;
    // Try to fit the x and y in the min and max range.
    // If it does not fit, the range is extended.
    // If |forceFit| is false, and the range was extended, then the function
    // immediately returns without changing the range.
    // If the range was extended and |forceFit| is true, then the desired ranges
    // are updated and the function returns true.
    if (tryToFitAtCoordinate(*x, &desiredMinX, &desiredMaxX)) {
        if (!forceFit) {
            return false;
        }
        if (desiredMaxX - desiredMinX >= VENUS_MAP_SIZE) {
            // The new range is bigger than the available memory.
            return false;
        }
    }
    if (tryToFitAtCoordinate(*y, &desiredMinY, &desiredMaxY)) {
        if (!forceFit) {
            return false;
        }
        if (desiredMaxY - desiredMinY >= VENUS_MAP_SIZE) {
            return false;
        }
    }

    minX = desiredMinX;
    maxX = desiredMaxX;
    minY = desiredMinY;
    maxY = desiredMaxY;
    // At this point, we know that (x,y) can be stored. Now adjust the
    // coordinates so that *x and *y can be used to access memory in the map.
    // See the comment at centerXY for the logic and reasoning.
    if (*x < centerXY) {
        *x = VENUS_MAP_SIZE - (centerXY - *x);
    } else {
        *x -= centerXY;
    }
    if (*y < centerXY) {
        *y = VENUS_MAP_SIZE - (centerXY - *y);
    } else {
        *y -= centerXY;
    }
    return true;
}

void addVisit(byte x, byte y) {
    if (!fitsInMap(&x, &y, true)) {
        return;
    }
    byte value = map[x / 2][y];
    // If the value is less than 7, then we can safely add one
    // without the carry messing up the other bits.
    if (x & 1) { // Odd number = right
        if ((value & 7) < 7) {
            map[x / 2][y] = value + 1;
        }
    } else { // Even number = left
        if (((value >> 4) & 7) < 7) {
            map[x / 2][y] = value + (1 << 4);
        }
    }
}

byte getVisits(byte x, byte y) {
    if (!fitsInMap(&x, &y)) {
        return 0;
    }
    if (x & 1) { // Odd number = right
        return map[x / 2][y] & 7;
    } else { // Even number = left
        return (map[x / 2][y] >> 4) & 7;
    }
}

void setObstacle(byte x, byte y) {
    if (!fitsInMap(&x, &y, true)) {
        return;
    }
    // Set bitfield: 8 = 0100
    if (x & 1) { // Odd number = right
        map[x / 2][y] |= 8;
    } else { // Even number = left
        map[x / 2][y] |= (8 << 4);
    }
}

bool hasObstacle(byte x, byte y) {
    if (!fitsInMap(&x, &y)) {
        // If the coordinates do not fit on the map, assume that the field is
        // clear.
        return false;
    }
    if (x & 1) { // Odd number = right
        return map[x / 2][y] & 8;
    } else { // Even number = left
        return map[x / 2][y] & (8 << 4);
    }
}

bool getSuggestion(byte x, byte y, byte* targetX, byte* targetY) {
    // TODO(rob): Implement.
    // ... calculate optimal choice, and then export the result ...
    // *targetX = ...;
    // *targetY = ...;
    // return true;
    return false;
}

byte toMapCoordinate(int position) {
    position /= INTS_PER_SQUARE;
    position += centerXY;
    // Clamp to extreme values to avoid underflow/overflow.
    if (position < 0) {
        return 0;
    }
    if (position > 0xFF) {
        return 0xFF;
    }
    // position is now in range [0, 255], so it can safely be cast to a byte.
    return position;
}

int fromMapCoordinate(byte coordinate) {
    int position = coordinate;
    position -= centerXY;
    position *= INTS_PER_SQUARE;
    return position;
}
