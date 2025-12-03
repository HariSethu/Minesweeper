#pragma once
#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H
//Include FLTK headers that will be used in the GameWindow class
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>
#include <vector>
#include "Board.h"

class GameWindow : public Fl_Window {
	public:
		//Constructor to initialize the game window with given dimensions and mine count
		GameWindow(int width, int height, int mineCount);

		//Destructor Function
		~GameWindow();


	private:
		Board gameBoard; //Model to track states of all cells in board
		std::vector<Fl_Button*> cellButtons; //Vector to hold buttons representing each cell
		
		//Callback functions for button clicks, game reset, settigns, and timer
		static void buttonCallback(Fl_Widget* widget, void* data); //Static callback function for button clicks
		static void settings_cb(Fl_Widget* widget, void* data); //Static callback for settings button
		static void new_game(Fl_Widget* widget, void* data); //Static callback for new game button
		static void timer_cb(void* data); //Static callback for timer updates
		
		
		void updateGUI(); //Function to update the GUI based on the board state
		void endGame(bool won, int explodeX = -1, int explodeY = -1); //Function to handle end of game scenarios
		int boardWidth; //Width of the board in cells
		int boardHeight; //Height of the board in cells
		
		
		//Contents for Widgets like MineCounter, Reset Button, Timer
		Fl_Output* mineCounterOutput; //Output widget to display remaining mine count
		Fl_Output* timerOutput; //Output widget to display elapsed time
		
		// Labels for the outputs
		Fl_Box* mineLabel; // Label for remaining mines
		Fl_Box* timerLabel; // Label for elapsed time
		

		Fl_Button* resetButton; //Button to reset the game
		Fl_Button* settingsButton; //Button to toggle flag mode

		// Layout helpers
		void layoutTopControls(int windowWidth);
		void layoutGrid(int windowWidth, int windowHeight);
		// Keep top controls repositioned when window is resized
		void resize(int X, int Y, int W, int H) override;

		Fl_Button* createCellButton(int x, int y); //Function to create a button for a cell at (x, y)
		Fl_Button* getButtonAt(int x, int y); //Function to get the button at cell (x, y)

		//Store Pointers to Images for Mines, Flags (Fl_PNG_Image is a Fl_Image subclass)
		Fl_PNG_Image* imgMine;
		Fl_PNG_Image* imgFlag;

		//Vairables for Timer Functionality
		int secondsElapsed; //Seconds elapsed since the start of the game
		bool timerRunning; //Flag to indicate if the timer is running
};

#endif