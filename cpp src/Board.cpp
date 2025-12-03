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
	this->flagsPlaced = 0; // initialize flagsPlaced to avoid undefined behavior

	//Initialize the board with empty cells
	cell.resize(height, std::vector<Cell>(width));

	// Seed RNG once (avoid reseeding every time placeMines is called).
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

//Places mines in random coordinates of the board
void Board::placeMines(int ClickX, int ClickY) {
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
int Board::getFlagsPlaced() const { return flagsPlaced; }


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
				// Skip mines when expanding flood-fill to avoid revealing them and triggering game over.
				if (cell[ny][nx].isMine) continue;
				// Recurse; function checks flags/revealed
				revealCell(nx, ny);
			}
		}
	}
	if (x < 0 || x >= width || y < 0 || y >= height) {return;}


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

	if (cell[y][x].isFlagged) {
		flagsPlaced++;
	} else {
		flagsPlaced--;
	}

	// Re-evaluate win condition in case player wins by correctly flagging all mines
	checkWinCondition();
}

void Board::checkWinCondition() {
	int totalCells = width * height;
	// Win if all non-mine cells are revealed
	if (cellsRevealed == totalCells - mineCount) {
		isGameWon = true;
		isGameOver = true;
		return;
	}

	// Alternate win: all mines are flagged and no non-mine is flagged
	if (flagsPlaced == mineCount) {
		int flaggedCount = 0;
		int correctFlags = 0;
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				if (cell[y][x].isFlagged) {
					++flaggedCount;
					if (cell[y][x].isMine) ++correctFlags;
				}
			}
		}
		if (flaggedCount == mineCount && correctFlags == mineCount) {
			isGameWon = true;
			isGameOver = true;
			return;
		}
	}
}

bool Board::getIsGameOver() const {
	return isGameOver;
}

bool Board::getIsGameWon() const {
	return isGameWon;
}

//Reset Board either when the user gives up/loses/wins
void Board::resetBoard() {
	//Reset values of all game conditions
	firstClickHandled = false;
	isGameOver = false;
	isGameWon = false;
	cellsRevealed = 0;
	flagsPlaced = 0;

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			//reset all values of each cell
			cell[y][x].isFlagged = false;
			cell[y][x].isMine = false;
			cell[y][x].isRevealed = false;
			cell[y][x].adjacentMines = 0;
		}
	}
}

void Board::chordCell(int x, int y) {
	//Basic checks to see if coordinates are valid and revealed
	if (x < 0 || x >= width || y < 0 || y >= height) {
		return;
	}
	Cell& currCell = cell[y][x];

	//Check if cell is revealed and has adjacent mines
	if (!currCell.isRevealed || currCell.adjacentMines == 0) {
		return;
	}

	//Count surronding flags
	const int directions[8][2] = {
		{-1, -1}, {-1, 0}, {-1, 1},
		{0, -1},          {0, 1},
		{1, -1}, {1, 0}, {1, 1}
	};
	int flagCount = 0;
	//Count flags around the cell
	for (const auto& dir : directions) {
		int newX = x + dir[0];
		int newY = y + dir[1];
		if (newX >= 0 && newX < width && newY >= 0 && newY < height) {
			if (cell[newY][newX].isFlagged) {
				flagCount++;
			}
		}
	}
	//If flags match adjacent mines, reveal unflagged neighbors
	if (flagCount == currCell.adjacentMines) {
		for (const auto& dir : directions) {
			int nx = x + dir[0];
			int ny = y + dir[1];
			if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
				//Only reveal unflagged cells
				if (!cell[ny][nx].isFlagged) {
					revealCell(nx, ny);
				}
			}
		}
	}
}