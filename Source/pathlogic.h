#pragma once
typedef unsigned char byte;

// unsigned to prevent negative numbers
typedef unsigned int DistanceType;
typedef signed char Coordinate;

// Path struct holds basic information about driven paths
struct path
{
	DistanceType distance;
	// int due SRAM limitations, otherwise consuming 32*PATH_ENTRIES bytes
	int angle;
	// map Coordinates optimized using the toMapCoordinate function in 'location'
	Coordinate mapX;
	Coordinate mapY;
};

// Find the middle index of the longest subsequence. See getClosestPath.
unsigned int getClosestPathIndex(path arrayData[], unsigned int arrayLength,
		bool min);

// Finds the longest sequence with the smallest distance, and returns the
// path at the middle of this sequence. For example:
// 100, 22, 22, 22, 22, 300, 22, 22
//      ^    ^       ^
//     low   |      high
//      return value (=center among the elements with the minimal distance).
path getClosestPath(path arrayData[], unsigned int arrayLength, bool min);
