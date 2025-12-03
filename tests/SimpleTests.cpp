#include <iostream>
#include <string>
#include "Board.h"

static int g_tests = 0;
static int g_fails = 0;

#define EXPECT_EQ(a,b) do { ++g_tests; auto va = (a); auto vb = (b); if (!(va == vb)) { ++g_fails; std::cout << "FAIL: " << __func__ << ":" << __LINE__ << " : " << #a << " != " << #b << " (" << va << " vs " << vb << ")\n"; } } while(0)
#define EXPECT_TRUE(x) do { ++g_tests; if (!(x)) { ++g_fails; std::cout << "FAIL: " << __func__ << ":" << __LINE__ << " : " << #x << " is false\n"; } } while(0)
#define EXPECT_FALSE(x) do { ++g_tests; if (x) { ++g_fails; std::cout << "FAIL: " << __func__ << ":" << __LINE__ << " : " << #x << " is true\n"; } } while(0)

void TestConstruction() {
    Board b(10, 8, 12);
    EXPECT_EQ(b.getWidth(), 10);
    EXPECT_EQ(b.getHeight(), 8);
    EXPECT_EQ(b.getMineCount(), 12);
    EXPECT_FALSE(b.getIsGameOver());
    EXPECT_FALSE(b.getIsGameWon());
    EXPECT_EQ(b.getFlagsPlaced(), 0);
}

void TestToggleFlagAndCount() {
    Board b(5,5,3);
    EXPECT_FALSE(b.getCell(1,2).isFlagged);
    b.toggleFlag(1,2);
    EXPECT_TRUE(b.getCell(1,2).isFlagged);
    EXPECT_EQ(b.getFlagsPlaced(), 1);
    b.toggleFlag(1,2);
    EXPECT_FALSE(b.getCell(1,2).isFlagged);
    EXPECT_EQ(b.getFlagsPlaced(), 0);
}

void TestResetClearsFlags() {
    Board b(6,6,5);
    b.toggleFlag(0,0);
    b.toggleFlag(2,3);
    // ensure flags were placed before reset
    EXPECT_EQ(b.getFlagsPlaced(), 2);
    b.resetBoard();
    EXPECT_EQ(b.getFlagsPlaced(), 0);
    for (int y = 0; y < b.getHeight(); ++y) {
        for (int x = 0; x < b.getWidth(); ++x) {
            const Cell &c = b.getCell(x,y);
            EXPECT_FALSE(c.isFlagged);
            EXPECT_FALSE(c.isRevealed);
        }
    }
    EXPECT_FALSE(b.getIsGameOver());
    EXPECT_FALSE(b.getIsGameWon());
}

void TestChordNoCrash() {
    Board b(8,8,10);
    // calling chordCell should not crash; behavior depends on board state
    b.chordCell(3,3);
    EXPECT_TRUE(true); // if we reach here, assume no crash
}

int main() {
    std::cout << "Running simple tests...\n";

    TestConstruction();
    TestToggleFlagAndCount();
    TestResetClearsFlags();
    TestChordNoCrash();

    std::cout << "Tests run: " << g_tests << ", Failures: " << g_fails << "\n";
    if (g_fails == 0) {
        std::cout << "ALL TESTS PASSED\n";
        return 0;
    } else {
        std::cout << "SOME TESTS FAILED\n";
        return 1;
    }
}