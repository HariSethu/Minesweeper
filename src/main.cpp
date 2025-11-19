#pragma comment(lib, "Gdiplus.lib")
#include <FL/Fl.H>
#include <iostream>
#include <sstream>
#include <string>
#include "GameWindow.h"
#include "Settings.h"

int main(int argc, char** argv){
	SettingsWindow* settingsWin = new SettingsWindow(600, 400, "Minesweeper Settings");
	settingsWin->show(argc, argv);

	return Fl::run();
}