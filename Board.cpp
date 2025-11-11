#include "Board.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <stdexcept>

//Constructor to initialize the board with given dimensions and mine count
Board::Board(int width, int height, int mineCount)
{
	//Set board dimensions and mine count
	this->width = width;
	this->height = height;
	this->mineCount = mineCount;
	this->firstClickHandled = false;
	this->isGameOver = false;
	this->isGameWon = false;
	this->cellsRevealed = 0;
	//Initialize the board with empty cells
	cell.resize(height, std::vector<Cell>(width));



}

//Places mines in random coordinates of the board
void Board::placeMines(int ClickX, int ClickY) {
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	int placedMines = 0;
	while (placedMines < mineCount) {
		int x = std::rand() % width;
		int y = std::rand() % height;
		//Avoid placing mine on the first clicked cell and sees if cell does not already contain a mine
		if ((x != ClickX || y != ClickY) && !cell[y][x].isMine) {
			cell[y][x].isMine = true;
			placedMines++;
		}
	}
    calculateAdjacentMines();
}

//Calculates the number of adjacent mines for each cell
void Board::calculateAdjacentMines()
{
	//Directions for the 8 neighboring cells
	const int directions[8][2] = {
		{-1, -1}, {-1, 0}, {-1, 1},
		{0, -1},          {0, 1},
		{1, -1}, {1, 0}, {1, 1}
	};

	//Iterate through each cell on the board and count adjacent mines
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (cell[y][x].isMine) continue;
			else
			{
				int mineCount = 0;
				//Checks all 8 directions for adjacent mines
				for (const auto& dir : directions)
				{
					int newX = x + dir[0];
					int newY = y + dir[1];
					if (newX >= 0 && newX < width && newY >= 0 && newY < height)
					{
						if (cell[newY][newX].isMine)
						{
							mineCount++;
						}
					}
				}
				//Sets count of adjacent mines for the cell
				cell[y][x].adjacentMines = mineCount;
			}
		}
	}
}

//Simple getters for board dimensions
int Board::getWidth() const 
{
	return width;
}
int Board::getHeight() const 
{
	return height;
}
int Board::getMineCount() const
{
	return mineCount;
}
const Cell& Board::getCell(int x, int y) const
{
	return cell[y][x];
}

//Reveals the cell at given coordinate when selected
void Board::revealCell(int x, int y) {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return;
	}

	if(!firstClickHandled) {
		placeMines(x, y);
		firstClickHandled = true;
	}

	Cell& current = cell[y][x];
	if (current.isRevealed || current.isFlagged) {
		return;
	}

	// If mine => game over
	if (current.isMine) {
		isGameOver = true;
		current.isRevealed = true;
		++cellsRevealed;
		return;
	}

	// Reveal this cell
	current.isRevealed = true;
	++cellsRevealed;

	// Only flood-fill neighbors if this cell has 0 adjacent mines
	if (current.adjacentMines == 0) {
		static const std::vector<std::pair<int, int>> dirs = {
			{-1, -1}, {-1, 0}, {-1, 1},
			{0, -1},           {0, 1},
			{1, -1},  {1, 0},  {1, 1}
		};
		for (const auto& dir : dirs) {
			int nx = x + dir.first;
			int ny = y + dir.second;
			if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
				// Recurse; function checks flags/revealed
				revealCell(nx, ny);
			}
		}
	}

	checkWinCondition();
}

//Can toggle flag on a cell to mark as a mine
void Board::toggleFlag(int x, int y) {
	if (x < 0 || x >= width || y < 0 || y >= height) {
		throw std::out_of_range("Cell coordinates is out of range!");
	}
	if(cell[y][x].isRevealed) {
		return; //Nothing happens, already revealed
	}


	//Else, Toggle flagged state of cell
	cell[y][x].isFlagged = !cell[y][x].isFlagged;
}

void Board::checkWinCondition() {
	int totalCells = width * height;
	if(cellsRevealed == totalCells - mineCount) {
		isGameWon = true;
		isGameOver = true;
	}
}

bool Board::getIsGameOver() const {
	return isGameOver;
}

bool Board::getIsGameWon() const {
	return isGameWon;
}
//End of Board.cpp