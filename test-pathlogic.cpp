#include "Source/pathlogic.h"
#include <stdio.h>

bool g_has_any_test_failure = false;

#define EXPECT_EQ(expected, actual) \
    do { \
        byte result = (actual);\
        if (result != expected) {\
            g_has_any_test_failure = true;\
            printf("Expectation failed: " #actual " should be " #expected \
                    ", but was %d\n", result);\
        }\
    } while (0)

void testGetClosestPath() {
    {
        path paths[13];
        paths[0].distance = 15;
        paths[1].distance = 9;
        paths[2].distance = 9; // <-- expected min
        paths[3].distance = 9;
        paths[4].distance = 20;
        paths[5].distance = 20;
        paths[6].distance = 9;
        paths[7].distance = 9;
        paths[8].distance = 20;
        paths[9].distance = 20; // <--- expected max
        paths[10].distance = 20;
        paths[11].distance = 20;
        paths[12].distance = 15;

        EXPECT_EQ(2, getClosestPathIndex(paths, 13, true));
        EXPECT_EQ(9, getClosestPathIndex(paths, 13, false));
    }

    {
        path paths[1];
        EXPECT_EQ(0, getClosestPathIndex(paths, 1, true));
        EXPECT_EQ(0, getClosestPathIndex(paths, 1, false));
    }

    {
        path paths[2];
        paths[0].distance = 1;
        paths[1].distance = 2;
        EXPECT_EQ(0, getClosestPathIndex(paths, 2, true));
        EXPECT_EQ(1, getClosestPathIndex(paths, 2, false));
    }

    {
        // This is the example from the documentation.
        path paths[8];
        paths[0].distance = 100;
        paths[1].distance = 22;
        paths[2].distance = 22; // <--
        paths[3].distance = 22;
        paths[4].distance = 22;
        paths[5].distance = 300;
        paths[6].distance = 22;
        paths[7].distance = 22;
        EXPECT_EQ(2, getClosestPathIndex(paths, 6, true));
        EXPECT_EQ(5, getClosestPathIndex(paths, 6, false));
    }
}

int main() {
    testGetClosestPath();
    if (g_has_any_test_failure) {
        return 1;
    }
    printf("The path logic test completed successfully!\n");
    return 0;
}
