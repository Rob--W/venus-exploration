#include "pathlogic.h"

// TODO(rob): The next comment is not entirely true, see the comments within the
// function for the actual behavior.
// Find the path with the smallest distance (but still larger than SAFE_DISTANCE).
// arrayData is an ordered list of measurements. If multiple paths have the same
// distance, then the center of these paths is returned.
byte getClosestPathIndex(path arrayData[], unsigned int arrayLength,
		bool min = true) {
	byte bestLow = 0;
	byte bestLength = 0;
	DistanceType extremeDistance = arrayData[0].distance;

	// TODO(rob): What if the distances is smaller than SAFE_DISTANCE?
	// Marijn said that the function is supposed to ignore paths with a distance
	// shorter than SAFE_DISTANCE. Should we adapt the next logic to implement
	// this requirement?

	// The next loop attempts to find the low index and the high index of the
	// range of extreme (lowest/highest) distances. In the end, the middle of
	// these values will be returned. For example:
	// 100, 22, 22, 22, 22, 300
	//      ^    ^       ^
	//     low   |      high
	//      return value (=center among the elements with the minimal distance).
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
