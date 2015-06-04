#include "location.h"

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

// TODO(rob): Translate coordinates, to maximize the use of the map.
// The map's maximum size is 255x255, but due to memory constraints, the actual
// maximum size is much smaller. To make sure that the space in the map is used
// optimally, we have to change the dx and dy value when an attempt is made to
// add a new visit / obstacle to the map beyond the boundaries.
//byte dx = 127;
//byte dy = 127;

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

void addVisit(byte x, byte y) {
    if (x > VENUS_MAP_SIZE || y > VENUS_MAP_SIZE) {
        // Ignore items that do not fit in the map.
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
    if (x > VENUS_MAP_SIZE || y > VENUS_MAP_SIZE) {
        return 0;
    }
    if (x & 1) { // Odd number = right
        return map[x / 2][y] & 7;
    } else { // Even number = left
        return (map[x / 2][y] >> 4) & 7;
    }
}

void setObstacle(byte x, byte y) {
    if (x > VENUS_MAP_SIZE || y > VENUS_MAP_SIZE) {
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
    if (x > VENUS_MAP_SIZE || y > VENUS_MAP_SIZE) {
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
    return position/10 + 20;
}

int fromMapCoordinate(byte coordinate) {
    return coordinate*10 - 200;
}
