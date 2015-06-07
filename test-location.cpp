#undef ONLY_FOR_TESTING_LOCATION_H
#define ONLY_FOR_TESTING_LOCATION_H
#include "Source/location.h"
#include <assert.h>
#include <stdio.h>

bool g_has_any_test_failure = false;

#define EXPECT_EQ(expected, actual) \
    do { \
        byte result = (actual);\
        if (result != expected) {\
            g_has_any_test_failure = true;\
            printf("Expectation failed: " #actual " should be " #expected \
                    ", but was %d at line %d\n", result, __LINE__);\
        }\
    } while (0)

void testFitInMap() {
    resetMap();
    // Initial values.
    EXPECT_EQ(centerXY, minX);
    EXPECT_EQ(centerXY, maxX);
    EXPECT_EQ(centerXY, minY);
    EXPECT_EQ(centerXY, maxY);

    // The center starts at index (0,0).
    byte x = centerXY;
    byte y = centerXY;
    assert(fitsInMap(&x, &y, false));
    EXPECT_EQ(0, x);
    EXPECT_EQ(0, y);

    // Does not fit, and false means out value for x should not be used.
    x = centerXY + 1;
    y = centerXY;
    assert(!fitsInMap(&x, &y, false));

    // Basic test for X
    x = centerXY + 1;
    y = centerXY;
    assert(fitsInMap(&x, &y, true));
    EXPECT_EQ(centerXY + 1, maxX);
    EXPECT_EQ(1, x);
    EXPECT_EQ(0, y);

    x = centerXY - 2;
    y = centerXY;
    assert(!fitsInMap(&x, &y, false));

    x = centerXY - 2;
    y = centerXY;
    assert(fitsInMap(&x, &y, true));
    EXPECT_EQ(centerXY - 2, minX); // Should fit
    EXPECT_EQ(centerXY + 1, maxX); // Should not be changed
    EXPECT_EQ(VENUS_MAP_SIZE - 2, x);
    EXPECT_EQ(0, y);

    // Basic test for Y
    x = centerXY;
    y = centerXY + 1;
    assert(fitsInMap(&x, &y, true));
    EXPECT_EQ(centerXY + 1, maxY);
    EXPECT_EQ(0, x);
    EXPECT_EQ(1, y);

    x = centerXY;
    y = centerXY - 2;
    assert(!fitsInMap(&x, &y, false));

    x = centerXY;
    y = centerXY - 2;
    assert(fitsInMap(&x, &y, true));
    EXPECT_EQ(centerXY - 2, minY); // Should not be changed
    EXPECT_EQ(centerXY + 1, maxY); // Should fit
    EXPECT_EQ(0, x);
    EXPECT_EQ(VENUS_MAP_SIZE - 2, y);

    minX = maxX = minY = maxY = centerXY; // Reset state
    // Off-by-one tests
    x = centerXY;
    y = centerXY + VENUS_MAP_SIZE - 1;
    assert(fitsInMap(&x, &y, true));
    EXPECT_EQ(centerXY + VENUS_MAP_SIZE - 1, maxY); // Should fit
    EXPECT_EQ(0, x);
    EXPECT_EQ(VENUS_MAP_SIZE - 1, y);

    x = centerXY;
    y = centerXY + VENUS_MAP_SIZE;
    assert(!fitsInMap(&x, &y, true));

    // There is no space any more.
    x = centerXY;
    y = centerXY - 1;
    assert(!fitsInMap(&x, &y, true));
}

void testRocks() {
    resetMap();
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
    resetMap();
    EXPECT_EQ(0, getVisits(2, 2));
    addVisit(2, 2);
    EXPECT_EQ(1, getVisits(2, 2));
    addVisit(2, 2);
    EXPECT_EQ(2, getVisits(2, 2));
    addVisit(2, 2);
    EXPECT_EQ(3, getVisits(2, 2));
    addVisit(2, 2);
    EXPECT_EQ(4, getVisits(2, 2));
    addVisit(2, 2);
    EXPECT_EQ(5, getVisits(2, 2));
    addVisit(2, 2);
    EXPECT_EQ(6, getVisits(2, 2));
    addVisit(2, 2);
    EXPECT_EQ(7, getVisits(2, 2));
    addVisit(2, 2);
    EXPECT_EQ(7, getVisits(2, 2));
    EXPECT_EQ(0, getVisits(1, 2));
    EXPECT_EQ(0, getVisits(3, 2));
}

void testObstacles() {
    resetMap();
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
    EXPECT_EQ(0, getVisits(0, 1));
    EXPECT_EQ(0, getVisits(1, 1));
    EXPECT_EQ(0, getVisits(1, 0));
}

void testConversions() {
    resetMap();
    // For now, just test that integers in the range [0,255] are preserved,
    // and that other values are truncated.
    assert(toMapCoordinate(100) == 100);
    assert(toMapCoordinate(257) == 1);
    assert(fromMapCoordinate(255) == 255);
}

int main() {
    testFitInMap();
    testRocks();
    testVisits();
    testObstacles();
    // Disabled.
    //testConversions();

    // We only have so many bytes.
    assert((VENUS_MAP_SIZE / 2) * VENUS_MAP_SIZE < 2048);
    // But we cannot use all memory, because other functions also need memory.
    assert((VENUS_MAP_SIZE / 2) * VENUS_MAP_SIZE < 1024);
    // Print minimum map size, because it will be the most significant memory
    // hog.
    printf("Minimum memory usage by this module: %d bytes.\n",
            (VENUS_MAP_SIZE / 2) * VENUS_MAP_SIZE);

    if (!g_has_any_test_failure) {
        printf("All tests have completed successfully!\n");
    }
    return 0;
}
