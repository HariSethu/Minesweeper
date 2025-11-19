#pragma once
#ifndef SETTINGS_H
#define SETTINGS_H

#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Int_Input.H>


class SettingsWindow : public Fl_Window {
	public:
		//Constructor to instantiate the settings window
		SettingsWindow(int width, int height, const char* title);
		~SettingsWindow();

	private:
		//Pointer for widgets
		Fl_Int_Input* widthInput;
		Fl_Int_Input* heightInput;
		Fl_Int_Input* mineInput;
		Fl_Button* startButton;

		Fl_Button* Beginner_btn;
		Fl_Button* Intermediate_btn;
		Fl_Button* Hard_btn;
		//Callback for when Start Game button is clicked and preset games
		static void start_cb(Fl_Widget* w, void* data);
		static void preset_cb(Fl_Widget* w, void* data);

};


#endif

