#pragma once

#ifndef BOARD_H
#define BOARD_H
#include <vector>

struct Cell {
	bool isMine;
	bool isRevealed;
	bool isFlagged;
	int adjacentMines;

	Cell() : isMine(false), isRevealed(false), isFlagged(false), adjacentMines(0) {}
};

class Board {
	public:
		Board(int width, int height, int mineCount);
		void revealCell(int x, int y);
		void toggleFlag(int x, int y);
		int getWidth() const;
		int getHeight() const;
		const Cell& getCell(int x, int y) const;
		int getMineCount() const;

        // Add these so header matches implementation
        bool getIsGameOver() const;
        bool getIsGameWon() const;

	private:
		int width;
		int height;
		int mineCount;
		std::vector<std::vector<Cell>> cell;
		void placeMines(int ClickX, int ClickY);
		void calculateAdjacentMines();
		void checkWinCondition();

		bool firstClickHandled;
		bool isGameOver;
		bool isGameWon;
		int cellsRevealed;
};

#endif