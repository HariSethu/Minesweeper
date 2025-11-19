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
	// Pass the preset string as the callback data (do not overwrite it later)
	Beginner_btn->callback(preset_cb, (void*)"9 9 10");

	//Intermediate Preset
	Intermediate_btn = new Fl_Button(320, 50, 70, 25, "Intermediate");
	Intermediate_btn->callback(preset_cb, (void*)"16 16 40");

	//Hard Preset
	Hard_btn = new Fl_Button(320, 80, 70, 25, "Hard");
	Hard_btn->callback(preset_cb, (void*)"30 16 99");

	//Start Game button
	startButton = new Fl_Button(150, 160, 150, 30, "Start Game");
	//Callback
	startButton-> callback(start_cb, this);
	end();
}

//Destructor
SettingsWindow::~SettingsWindow() {}

//Callback for the preset buttons
void SettingsWindow::preset_cb(Fl_Widget* w, void* data)
{
	//Find the SettingsWindow from the widget hierarchy (the button's parent is the window).
	SettingsWindow* sw = static_cast<SettingsWindow*>(w->parent());
	if (!sw) return;

	// The callback data is the preset string we passed when registering the callback.
	const char* preset_cstr = static_cast<const char*>(data);
	if (!preset_cstr) return;

	int width = 0, height = 0, mines = 0;
	sscanf_s(preset_cstr, "%d %d %d", &width, &height, &mines);

	// Convert to string temporaries and set the input values.
	// Fl_Input::value copies the passed C-string, so temporaries are OK here.
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
	//Get width, height, and mine count from input fields
	int width = std::atoi(sw->widthInput->value());
	int height = std::atoi(sw->heightInput->value());
	int mines = std::atoi(sw->mineInput->value());

	//Error Handling for invalid User inputs
	if (width < 5) { width = 5; }
	if (height < 5) { height = 5; }
	if (mines < 1) { mines = 5; }
	if (mines >= width * height) { mines = width * height - 1; }


	//Create GameWindow with user defined settings
	GameWindow* gw = new GameWindow(width, height, mines);
	//Close settings window
	sw->hide();
}