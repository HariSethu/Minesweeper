#include "Settings.h"
#include "GameWindow.h"
#include <string>
#include <cstdlib>

//Constructor to instantiate the start settings window
SettingsWindow::SettingsWindow(int width, int height, const char* title) : Fl_Window(width, height, title)
{
	//Input fields for width, height, and mine count
	widthInput = new Fl_Int_Input(150, 20, 150, 25, "Width:");
	heightInput = new Fl_Int_Input(150, 50, 150, 25, "Height:");
	mineInput = new Fl_Int_Input(150, 80, 150, 25, "Mines:");

	//Default values
	widthInput->value("9");
	heightInput->value("9");
	mineInput->value("5");

	//Preset Values Buttons
	Beginner_btn = new Fl_Button(320, 20, 70, 25, "Beginner");
	Beginner_btn->callback(preset_cb, (void*)"9 9 10");

	//Intermediate Preset
	Intermediate_btn = new Fl_Button(320, 50, 70, 25, "Intermediate");
	Intermediate_btn->callback(preset_cb, (void*)"16 16 40");

	//Hard Preset
	Hard_btn = new Fl_Button(320, 80, 70, 25, "Hard");
	Hard_btn->callback(preset_cb, (void*)"30 16 99");

	//Start Game button
	startButton = new Fl_Button(150, 160, 150, 30, "Start Game");
	startButton-> callback(start_cb, this);
	end();
}

//Destructor
SettingsWindow::~SettingsWindow() {}

//Callback for the preset buttons
void SettingsWindow::preset_cb(Fl_Widget* w, void* data)
{
	SettingsWindow* sw = static_cast<SettingsWindow*>(w->parent());
	if (!sw) return;

	const char* preset_cstr = static_cast<const char*>(data);
	if (!preset_cstr) return;

	int width = 0, height = 0, mines = 0;
	sscanf_s(preset_cstr, "%d %d %d", &width, &height, &mines);

	std::string wstr = std::to_string(width);
	std::string hstr = std::to_string(height);
	std::string mstr = std::to_string(mines);

	if (sw->widthInput)  sw->widthInput->value(wstr.c_str());
	if (sw->heightInput) sw->heightInput->value(hstr.c_str());
	if (sw->mineInput)   sw->mineInput->value(mstr.c_str());
}


//Callback for when Start Game is clicked
void SettingsWindow::start_cb(Fl_Widget* w, void* data)
{
	//Retrieve the current SettingsWindow instance
	
	SettingsWindow* sw = static_cast<SettingsWindow*>(data);
	if (!sw) return;

	//Get width, height, and mine count from input fields
	int width = std::atoi(sw->widthInput->value());
	int height = std::atoi(sw->heightInput->value());
	int mines = std::atoi(sw->mineInput->value());

	// If dimensions exceed 100x100, or mines invalid relative to board size,
	// reset to default 9x9 with 5 mines.
	if (width > 100 || height > 100) {
		width = 9;
		height = 9;
		mines = 5;
	}
	// Enforce minimums (keep original behavior)
	if (width < 5) { width = 5; }
	if (height < 5) { height = 5; }

	// If mines invalid (less than 1) default to 5
	if (mines < 1) { mines = 5; }

	// If mines would exceed or equal board size, reset entire settings to 9x9/5 as requested
	if (mines >= width * height) {
		width = 9;
		height = 9;
		mines = 5;
	}

	//Create GameWindow with user defined settings
	GameWindow* gw = new GameWindow(width, height, mines);
	//Close settings window
	sw->hide();
}