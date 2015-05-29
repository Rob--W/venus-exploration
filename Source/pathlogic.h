#pragma once
typedef unsigned char byte;

// unsigned to prevent negative numbers
typedef unsigned int DistanceType;

// Path struct holds basic information about driven paths
struct path
{
	DistanceType distance;
	// int due SRAM limitations, otherwise consuming 32*PATH_ENTRIES bytes
	int angle;
};

// For testing. Equivalent to arrayData[getClosestPath( ... )]
byte getClosestPathIndex(path arrayData[], unsigned int arrayLength, bool min);

// Use this.
path getClosestPath(path arrayData[], unsigned int arrayLength, bool min);
