#include "Source/location.h"
#include <assert.h>
#include <stdio.h>

void testRocks() {
    // Sanity check.
    assert(!hasRock(0, 0));
    assert(!hasRock(100, 100));
    assert(!hasRock(255, 255));

    // Adding two rocks.
    addRock(0, 0);
    assert(hasRock(0, 0));
    addRock(0, 0);
    assert(hasRock(0, 0));
    // Remove one rock. There should still be one rock.
    removeRock(0, 0);
    assert(hasRock(0, 0));
    // Remove secondd rock. All rocks should have been removed.
    removeRock(0, 0);
    assert(!hasRock(0, 0));
    // Trying to remove a non-existent rock is allowed, nothing happens.
    removeRock(0, 0);
    assert(!hasRock(0, 0));

    // Non-zero coordinates, just in case.
    addRock(2, 100);
    assert(hasRock(2, 100));
    removeRock(2, 100);
    assert(!hasRock(2, 100));
}

void testVisits() {
    assert(getVisits(2, 2) == 0);
    addVisit(2, 2);
    assert(getVisits(2, 2) == 1);
    addVisit(2, 2);
    assert(getVisits(2, 2) == 2);
    addVisit(2, 2);
    assert(getVisits(2, 2) == 3);
    addVisit(2, 2);
    assert(getVisits(2, 2) == 4);
    addVisit(2, 2);
    assert(getVisits(2, 2) == 5);
    addVisit(2, 2);
    assert(getVisits(2, 2) == 6);
    addVisit(2, 2);
    assert(getVisits(2, 2) == 7);
    addVisit(2, 2);
    assert(getVisits(2, 2) == 7);
    assert(getVisits(1, 2) == 0);
    assert(getVisits(3, 2) == 0);
}

void testObstacles() {
    // Base case
    assert(!hasObstacle(1, 0));
    assert(!hasObstacle(1, 1));
    assert(!hasObstacle(1, 2));
    assert(!hasObstacle(2, 1));
    assert(!hasObstacle(3, 1));

    // Setting one item should set only one obstacle.
    setObstacle(1, 1);
    assert(!hasObstacle(1, 0));
    assert(hasObstacle(1, 1));
    assert(!hasObstacle(1, 2));
    assert(!hasObstacle(2, 1));
    assert(!hasObstacle(3, 1));

    // Setting another items should not modify other values.
    setObstacle(1, 0);
    assert(hasObstacle(1, 0));
    assert(hasObstacle(1, 1));
    assert(!hasObstacle(1, 2));
    assert(!hasObstacle(2, 1));
    assert(!hasObstacle(3, 1));

    // Also check visits, because the visit and obstacle bitfields are combined.
    assert(getVisits(0, 1) == 0);
    assert(getVisits(1, 1) == 0);
    assert(getVisits(1, 0) == 0);
}

void testConversions() {
    // For now, just test that integers in the range [0,255] are preserved,
    // and that other values are truncated.
    assert(toMapCoordinate(100) == 100);
    assert(toMapCoordinate(257) == 1);
    assert(fromMapCoordinate(255) == 255);
}

int main() {
    testRocks();
    testVisits();
    testObstacles();
    testConversions();

    // We only have so many bytes.
    assert((VENUS_MAP_SIZE / 2) * VENUS_MAP_SIZE < 2048);
    // But we cannot use all memory, because other functions also need memory.
    assert((VENUS_MAP_SIZE / 2) * VENUS_MAP_SIZE < 1024);
    // Print minimum map size, because it will be the most significant memory
    // hog.
    printf("Minimum memory usage by this module: %d bytes.\n",
            (VENUS_MAP_SIZE / 2) * VENUS_MAP_SIZE);

    printf("All tests have completed successfully!\n");
    return 0;
}
