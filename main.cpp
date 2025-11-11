#pragma comment(lib, "Gdiplus.lib")
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include "Board.h"

static void printBoard(const Board& b, bool showMines) {
	for (int y = 0; y < b.getHeight(); ++y) {
		for (int x = 0; x < b.getWidth(); ++x) {
			const Cell& c = b.getCell(x, y);
			if (showMines && c.isMine) std::cout << '*';
			else if (c.isRevealed) {
				if (c.adjacentMines > 0) std::cout << c.adjacentMines;
				else std::cout << ' ';
			}
			else if (c.isFlagged) std::cout << 'F';
			else std::cout << '.';
			std::cout << ' ';
		}
		std::cout << '\n';
	}
}

int main() {
	const int W = 8, H = 6, MINES = 8;
	Board board(W, H, MINES);

	std::cout << "Board before first click (no mines placed yet):\n";
	printBoard(board, /*showMines=*/false);

	// First reveal should place mines avoiding (2,1)
	int clickX = 2, clickY = 1;
	board.revealCell(clickX, clickY);

	std::cout << "\nBoard after first click at (" << clickX << ',' << clickY << ") - mines shown for debug:\n";
	printBoard(board, /*showMines=*/true);

	// Basic checks
	int mineCountFound = 0;
	int revealedNonMines = 0;
	int totalCells = W * H;
	for (int y = 0; y < H; ++y) {
		for (int x = 0; x < W; ++x) {
			const Cell& c = board.getCell(x, y);
			if (c.isMine) ++mineCountFound;
			if (!c.isMine && c.isRevealed) ++revealedNonMines;
		}
	}
	std::cout << "\nMines placed: " << mineCountFound << " (expected " << MINES << ")\n";
	assert(mineCountFound == MINES && "Unexpected mine count after first click");

	// Clicked cell must not be a mine and should be revealed
	const Cell& clicked = board.getCell(clickX, clickY);
	assert(!clicked.isMine && "First-click cell was a mine");
	assert(clicked.isRevealed && "First-click cell was not revealed");

	std::cout << "Revealed non-mine cells after initial reveal: " << revealedNonMines
	          << " of " << (totalCells - MINES) << '\n';

	// Toggle flag on a found mine, verify flagging toggles
	int mx = -1, my = -1;
	for (int y = 0; y < H && mx == -1; ++y)
		for (int x = 0; x < W; ++x)
			if (board.getCell(x, y).isMine) { mx = x; my = y; break; }

	assert(mx != -1 && "No mine found to test flagging");
	board.toggleFlag(mx, my);
	assert(board.getCell(mx, my).isFlagged && "Flagging failed");
	board.toggleFlag(mx, my);
	assert(!board.getCell(mx, my).isFlagged && "Unflagging failed");

	// Reveal all non-mine cells to simulate a win (no direct getIsGameWon in header)
	for (int y = 0; y < H; ++y)
		for (int x = 0; x < W; ++x)
			if (!board.getCell(x, y).isMine)
				board.revealCell(x, y);

	// Re-count revealed non-mine cells
	int revealedNow = 0;
	for (int y = 0; y < H; ++y)
		for (int x = 0; x < W; ++x)
			if (!board.getCell(x, y).isMine && board.getCell(x, y).isRevealed)
				++revealedNow;

	std::cout << "Revealed non-mine cells after revealing all non-mines: " << revealedNow
	          << " of " << (totalCells - MINES) << '\n';
	assert(revealedNow == (totalCells - MINES) && "Not all non-mine cells revealed; win condition not reached by reveals");

	std::cout << "\nAll smoke tests passed. To run, build and run the console target.\n";
	return 0;
}