#include "pathlogic.h"

byte getClosestPathIndex(path arrayData[], unsigned int arrayLength,
		bool min = true) {
	byte bestLow = 0;
	byte bestLength = 0;
	DistanceType extremeDistance = arrayData[0].distance;

	// The next loop attempts to finds the longest sequence with the smallest
	// distance, and returns the position of the middle of this sequence.
	// (if min == false, then obviously the largest distance is sought).
	for (unsigned int i = 0; i < arrayLength; ++i) {
		DistanceType distance = arrayData[i].distance;
		if (min ? distance > extremeDistance : distance < extremeDistance) {
			// When looking for the smallest distance, we are not interested in
			// paths with a higher distance, so skip the path.
			continue;
		}
		if (distance != extremeDistance) {
			// Found a new extreme (lowest or highest) value.
			extremeDistance = distance;
			bestLow = i;
			bestLength = 0;
		}
		byte currentLength = 0;
		for (unsigned int j = i + 1; j < arrayLength; ++j) {
			distance = arrayData[j].distance;
			if (distance != extremeDistance) {
				// We have found the end of the sequence of paths with identical
				// distance, so save the previous index and start looking for
				// other distances (in the outer loop).
				i = j - 1;
				break;
			}
			if (++currentLength > bestLength) {
				bestLow = i;
				bestLength = currentLength;
			};
		}
	}

	return bestLow + bestLength / 2;
}

path getClosestPath(path arrayData[], unsigned int arrayLength, bool min = true)
{
	return arrayData[getClosestPathIndex(arrayData, arrayLength, min)];
}
