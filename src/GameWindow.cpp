#include "GameWindow.h"
#include "Settings.h"
#include "Board.h"
#include <sstream>
#include <string>
#include <FL/fl_ask.H>
#include <iomanip>
#include <FL/fl_draw.H>
#include <FL/Fl_Image.H>

//Initialize GameWindow with board dimensions and mine count

GameWindow::GameWindow(int width, int height, int mineCount)
: Fl_Window(width * 30, (height * 30)+60, "Minesweeper"), boardWidth(width), boardHeight(height), gameBoard(width, height, mineCount)
{
	begin();
	int window = width * 30;
	int center = window / 2;

	// Increase top area so the board moves down more to allow space for widgets
	const int topControlsHeight = 120; 

	//Timer Initialization
	secondsElapsed = 0;
	timerRunning = false;

	//Load Images for Mines and Flags
	imgFlag = new Fl_PNG_Image("flag.png");
	imgMine = new Fl_PNG_Image("mine.png");


	//Mine Counter Label and Output (stacked: label above output)
	mineLabel = new Fl_Box(10, 8, 60, 15, "Remaining");
	mineLabel->labelsize(12);
	mineLabel->align(FL_ALIGN_CENTER);

	mineCounterOutput = new Fl_Output(10, 8 + 15 + 4, 60, 30);
	mineCounterOutput->color(FL_BLACK);
	mineCounterOutput->textcolor(FL_RED);
	mineCounterOutput->textsize(18);
	mineCounterOutput->value(std::to_string(mineCount).c_str());


	//Reset Button
	Fl_Button* resetBtn = new Fl_Button(center - 45, 10 + 12, 80, 30, "Reset");
	// store into member so other code can reference it if needed
	resetButton = resetBtn;
	resetBtn->callback(new_game, this);

	// Menu/Newgame Button
	settingsButton = new Fl_Button(center + 50, 10 + 12, 80, 30, "Settings");
	settingsButton->callback(settings_cb, this);
	
	//Timer Label and Output
	// compute positions relative to mineCounterOutput placement to ensure they share X axis
	const int mined_label_y = 8;
	const int mined_output_y = mined_label_y + 15 + 4; // 27
	const int mined_output_h = 30;
	const int stack_gap = 6;
	const int timer_label_y = mined_output_y + mined_output_h + stack_gap; // stacked below mine counter
	const int timer_output_y = timer_label_y + 15 + 4; // below timer label

	timerLabel = new Fl_Box(10, timer_label_y, 60, 15, "Time");
	timerLabel->labelsize(12);
	timerLabel->align(FL_ALIGN_CENTER);

	timerOutput = new Fl_Output(10, timer_output_y, 60, 30);
	timerOutput->color(FL_BLACK);
	timerOutput->textcolor(FL_RED);
	timerOutput->textsize(18);
	timerOutput->value("000");

	//Create buttons for each cell in the board (moved down by topControlsHeight)
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			Fl_Button* btn = new Fl_Button(x * 30, (y * 30) + topControlsHeight, 30, 30);
			btn->callback(buttonCallback, this);
			// keep images/labels centered in the cell
			btn->align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);
			cellButtons.push_back(btn);
		}
	}

	// Ensure top widgets positioned in case window() differs from computed 'window'
	layoutTopControls(window);

	end();
	show();
}

//Destructor
GameWindow::~GameWindow() {
	//Buttons are automatically deleted by FLTK when the window is destroyed

	delete imgFlag;
	delete imgMine;

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
	
	//Get cell state to see if its revealed or hidden
	const Cell& cell = gw->gameBoard.getCell(x, y);
	int button = Fl::event_button();


	//Start Timer on first valid click on board
	if(!gw->timerRunning && !gw->gameBoard.getIsGameOver() && !gw->gameBoard.getIsGameWon()) {
		if (button == FL_LEFT_MOUSE && !cell.isFlagged) {
			gw->timerRunning = true;
			Fl::add_timeout(1.0, timer_cb, gw);
		}

	}

	//Checks if cell is already revealed or hidden
	if (cell.isRevealed) {
		//if revealed, left/right does nothing but middle click chords
		if (button == FL_MIDDLE_MOUSE) {
			gw->gameBoard.chordCell(x, y);
		}
	}
	//Else if the cell is hidden
	else {
		if(button == FL_LEFT_MOUSE) {
			// Prevent revealing flagged cells at GUI level as well
			if (!cell.isFlagged) {
				gw->gameBoard.revealCell(x, y);
			}
		} 
		else if (button == FL_RIGHT_MOUSE) {
			gw->gameBoard.toggleFlag(x, y);
		}
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

	//reset timer
	gw->timerRunning = false;
	Fl::remove_timeout(timer_cb, gw);
	gw->secondsElapsed = 0;
	gw->timerOutput->value("000");

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
			btn->image(nullptr);
			btn->labelcolor(FL_BLACK);
			btn->color(FL_BACKGROUND_COLOR);
			btn->box(FL_UP_BOX);
			btn->redraw();
		}
	}
	//Update mine counter
	gw->mineCounterOutput->value(std::to_string(gw->gameBoard.getMineCount()).c_str());
	gw->redraw(); //Helps redraw window after resetting to prevent Win/Loss message
}

//Callback for "Menu" (Closes and opens Settings Window)
void GameWindow::settings_cb(Fl_Widget* widget, void* data) {
	GameWindow* gw = static_cast<GameWindow*>(data);

	gw->timerRunning = false;
	Fl::remove_timeout(timer_cb, gw);

	gw->hide(); //Hide current game window
	SettingsWindow* settings = new SettingsWindow(600, 400, "Minesweeper Settings");
	settings->show(); //Show settings window
}

//Timer tick function to update elapsed time
void GameWindow::timer_cb(void* data) {
	GameWindow* gw = static_cast<GameWindow*>(data);
	if (!gw->timerRunning) return; //Stop timer if not running

	gw->secondsElapsed++;
	//Update timer output with 3 digit format
	std::stringstream ss;
	ss << std::setw(3) << std::setfill('0') << gw->secondsElapsed;
	gw->timerOutput->value(ss.str().c_str());

	//Recall timer after 1 second
	Fl::repeat_timeout(1.0, timer_cb, gw);
}

//Update the GUI based on the board state
void GameWindow::updateGUI() {

	//Update conter based on mines and flags placed
	int remaining = gameBoard.getMineCount() - gameBoard.getFlagsPlaced();
	mineCounterOutput->value(std::to_string(remaining).c_str());

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
			//To ensure no flag/mine image is shown on revealed cells
			if(btn->image() && !c.isMine) btn->image(nullptr);

			if (btn->box() != FL_FLAT_BOX) btn->box(FL_FLAT_BOX);
			if (c.isMine) {
				if (imgMine) {
					btn->image(imgMine);
					btn->copy_label(""); // No text needed if image exists
				}
				else {
					btn->copy_label("*");
					btn->labelcolor(FL_RED);
				}
			} else if (c.adjacentMines > 0) {
				std::string s = std::to_string(c.adjacentMines);
				btn->copy_label(s.c_str()); //Show adjacent mine count
				switch (c.adjacentMines) {
					case 1: btn->labelcolor(FL_BLUE); break;
					case 2: btn->labelcolor(FL_GREEN); break;
					case 3: btn->labelcolor(FL_RED); break;
					default: btn->labelcolor(FL_DARK_BLUE); break;
				}
			} else {
				btn->copy_label(""); //Empty for no adjacent mines
			}
		}
		else {
			// Unrevealed cells: show flag if flagged, otherwise blank
			if (btn->box() != FL_UP_BOX) btn->box(FL_UP_BOX);
			if (!btn->active()) btn->activate();
			if (c.isFlagged) {
				// USE IMAGE FOR FLAG
				if (imgFlag) {
					btn->image(imgFlag);
					btn->copy_label("");
				}
				else {
					btn->copy_label("F");
					btn->labelcolor(FL_RED);
				}
			} else {
				 //clear any previous flag image when flag removed
				if (btn->image()) btn->image(static_cast<Fl_Image*>(nullptr));
				btn->activate();
				btn->copy_label("");
				btn->labelcolor(FL_BLACK); // reset any leftover label color
			}
		}
		btn->redraw(); //Redraw button to reflect changes
	}
}

//Helper to display end of game message
void GameWindow::endGame(bool won, int explodeX, int explodeY) {
	// Reveal all mines and disable all buttons to prevent further operation
	//Stop timer
	timerRunning = false;
	Fl::remove_timeout(timer_cb, this);

	for (int i = 0; i < boardWidth * boardHeight; ++i) {
		int x = i % boardWidth;
		int y = i / boardWidth;
		const Cell& c = gameBoard.getCell(x, y);
		Fl_Button* btn = cellButtons[i];
		if (c.isMine) {
			// Show mine image
			if (imgMine) btn->image(imgMine);
			else {
				btn->copy_label("*");
				btn->labelcolor(FL_RED);
			}
			if (x == explodeX && y == explodeY) {
				btn->color(FL_RED); // Red background for explosion
			}

		}
		btn->deactivate();
	}
	this->redraw();

	// Create a centered message box inside this window to show result
	const char* msg = won ? "You Win!" : "You Lose!";
	fl_message("%s", msg);

	
}

// Reposition top controls based on window width.
void GameWindow::layoutTopControls(int windowWidth) {
	// Restore original positions but stack label above output for counter and timer.
	const int margin = 10;
	const int labelH = 15;
	const int outputW = 60;
	const int outputH = 30;
	const int topControlsHeight = 90; // must match constructor's offset

	// compute center using current window width
	int center = windowWidth / 2;

	// base y for the stacked left column (matches constructor layout)
	int baseY = topControlsHeight - 82; // keeps same vertical feel as constructor

	// Mine counter (left) - stacked (label above output)
	if (mineLabel) mineLabel->resize(margin, baseY, outputW, labelH);
	if (mineCounterOutput) mineCounterOutput->resize(margin, baseY + labelH + 4, outputW, outputH);

	// Timer stacked under Remaining (same X axis)
	int timerBase = baseY + labelH + 4 + outputH + 6;
	if (timerLabel) timerLabel->resize(margin, timerBase, outputW, labelH);
	if (timerOutput) timerOutput->resize(margin, timerBase + labelH + 4, outputW, outputH);

	// Reset (center) - keep original horizontal center, vertical similar to before
	if (resetButton) resetButton->resize(center - 45, 10 + 12, 80, 30);

	// Settings (center right)
	if (settingsButton) settingsButton->resize(center + 50, 10 + 12, 80, 30);
}

// Ensure top controls are repositioned when window is resized
void GameWindow::resize(int X, int Y, int W, int H) {
	Fl_Window::resize(X, Y, W, H);
	layoutTopControls(W);
	redraw();	
}

//End of GameWindow.cpp