#include "GameWindow.h"
#include "Settings.h"
#include "Board.h"
#include <sstream>
#include <string>
#include <FL/fl_ask.H>

//Initialize GameWindow with board dimensions and mine count

GameWindow::GameWindow(int width, int height, int mineCount)
: Fl_Window(width * 30, (height * 30)+50, "Minesweeper"), boardWidth(width), boardHeight(height), gameBoard(width, height, mineCount)
{
	begin();
	int window = width * 30;
	int center = window / 2;

	//Reset Button
	Fl_Button* resetBtn = new Fl_Button(center - 45, 10, 80, 30, "Reset");
	// store into member so other code can reference it if needed
	resetButton = resetBtn;
	resetBtn->callback(new_game, this);

	//Menu/Newgame Button (Call back to Settings window)
	settingsButton = new Fl_Button(center + 50, 10, 80, 30, "Settings");
	settingsButton->callback(settings_cb, this);
	
	//Create buttons for each cell in the board
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			Fl_Button* btn = new Fl_Button(x * 30, (y * 30)+50, 30, 30);
			btn->callback(buttonCallback, this);
			cellButtons.push_back(btn);
		}
	}
	end();
	show();
}

//Destructor
GameWindow::~GameWindow() {
	//Buttons are automatically deleted by FLTK when the window is destroyed
}

//Static callback for button clicks
void GameWindow::buttonCallback(Fl_Widget* widget, void* data) {
	//Retrieve the current GameWindow instance from user data
	GameWindow* gw = static_cast<GameWindow*>(data);
	Fl_Button* btn = static_cast<Fl_Button*>(widget);
	int index = -1;


	//Find the index of the clicked button
	for (size_t i = 0; i < gw->cellButtons.size(); ++i) {
		if (gw->cellButtons[i] == btn) {
			index = static_cast<int>(i);
			break;
		}
	}

	//Case: Button not found
	if (index == -1) return; 

	//Calculate cell coordinates from index
	int x = index % gw->boardWidth;
	int y = index / gw->boardWidth;


	//Handle left and right clicks
	if (Fl::event_button() == FL_LEFT_MOUSE) {
		// Prevent revealing flagged cells at GUI level as well
		if (!gw->gameBoard.getCell(x, y).isFlagged) {
			gw->gameBoard.revealCell(x, y);
		}
	} else if (Fl::event_button() == FL_RIGHT_MOUSE) {
		gw->gameBoard.toggleFlag(x, y);
	}
	//Update the GUI after the action
	gw->updateGUI();

	// Check win first, then loss.
	// Board sets both isGameWon and isGameOver on a win, so checking isGameOver first produced "You Lose!" even when won.
	if (gw->gameBoard.getIsGameWon()) {
		gw->endGame(true);
	} else if (gw->gameBoard.getIsGameOver()) {
		gw->endGame(false, x, y);
	}
}

//callback for newgame button
void GameWindow::new_game(Fl_Widget* widget, void* data){
	GameWindow* gw = static_cast<GameWindow*>(data);

	gw->gameBoard.resetBoard();

	// Use direct indexing into cellButtons instead of calling a non-existent getButtonAt()
	for(int y = 0; y < gw->boardHeight; ++y){
		for(int x = 0; x < gw->boardWidth; ++x){
			int idx = y * gw->boardWidth + x;
			// safety check
			if (idx < 0 || idx >= static_cast<int>(gw->cellButtons.size())) continue;
			Fl_Button* btn = gw->cellButtons[idx];
			if (!btn) continue;
			btn->activate();
			btn->copy_label("");
			btn->labelcolor(FL_BLACK);
			btn->color(FL_BACKGROUND_COLOR);
			btn->redraw();
		}
	}
	gw->redraw(); //Helps redraw window after resetting to prevent Win/Loss message
}

//Callback for "Menu" (Closes and opens Settings Window)
void GameWindow::settings_cb(Fl_Widget* widget, void* data) {
	GameWindow* gw = static_cast<GameWindow*>(data);

	gw->hide(); //Hide current game window
	SettingsWindow* settings = new SettingsWindow(600, 400, "Minesweeper Settings");
	settings->show(); //Show settings window
}
//Update the GUI based on the board state
void GameWindow::updateGUI() {
	//Iterate through each cell to update state of each button
	for (int i = 0; i < boardWidth * boardHeight; ++i)
	{
		//Calculate cell coordinates
		int x = i % boardWidth;
		int y = i / boardWidth;
		const Cell& c = gameBoard.getCell(x, y);
		//Update appearance based on cell state
		Fl_Button* btn = cellButtons[i];

		// If the cell is revealed, show its content and disable the button.
		if (c.isRevealed) {
			btn->deactivate(); // prevent further clicks on revealed cells
			if (c.isMine) {
				btn->copy_label("*"); //Show mine
				btn->labelcolor(FL_RED); //Mines in red
			} else if (c.adjacentMines > 0) {
				std::string s = std::to_string(c.adjacentMines);
				btn->copy_label(s.c_str()); //Show adjacent mine count
			} else {
				btn->copy_label(""); //Empty for no adjacent mines
			}
		}
		else {
			// Unrevealed cells: show flag if flagged, otherwise blank
			if (c.isFlagged) {
				btn->activate();
				btn->copy_label("F");
			} else {
				btn->activate();
				btn->copy_label("");
			}
		}
	}
}

//Helper to display end of game message
void GameWindow::endGame(bool won, int explodeX, int explodeY) {
	// Reveal all mines and disable all buttons to prevent further operation
	for (int i = 0; i < boardWidth * boardHeight; ++i) {
		int x = i % boardWidth;
		int y = i / boardWidth;
		const Cell& c = gameBoard.getCell(x, y);
		Fl_Button* btn = cellButtons[i];
		if (c.isMine) {
			btn->copy_label("*");
			//highlighs the mine that caused the explosion
			if( x == explodeX && y == explodeY) {
				btn->color(FL_YELLOW);
			}
			else {
				btn->labelcolor(FL_RED);
			}

		}
		btn->deactivate();
	}
	this->redraw();

	// Create a centered message box inside this window to show result
	const char* msg = won ? "You Win!" : "You Lose!";
	fl_message("%s", msg);

	
}

//End of GameWindow.cpp