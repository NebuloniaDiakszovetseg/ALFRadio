#include "Console.h"
#include <iostream>

namespace ng {
	HANDLE Console::HConsole_ = GetStdHandle(STD_OUTPUT_HANDLE);
	int Console::defaultColors_ = GetConsoleTextAttribute(HConsole_);
	std::hash<std::string> Console::strHash_;
	std::vector<size_t> Console::strHashes_;
	std::ostringstream Console::outStringStream_;

	void Console::log(const std::string& output, const FLAG& flag)
	{
		// convert once flags to normals
		FLAG realFlag = flag;
		bool once = false;
		if (realFlag > FLAG::FLAG_ERROR) {
			once = true;
			FLAG realFlag = FLAG(flag - FLAG::FLAG_ERROR);
		}

		if (once) {
			// get the hash for the output string
			size_t outputHash = strHash_(output);
			// search for hash in the container vector
			auto it = find(strHashes_.begin(), strHashes_.end(), outputHash);

			// if no hash was found 
			if (it == strHashes_.end())
				strHashes_.push_back(outputHash); // add new hash to vector (later log)
			else
				return; // else jump out of the program
		}

		// --- Actual Printing -------
		// Change font color based on severity
		if (realFlag == FLAG::FLAG_NOTE) {
			SetConsoleTextAttribute(HConsole_, defaultColors_);

			if (!Timer::getSysMeasured())
				std::cout << "NOTE: ";
		}
		else if (realFlag == FLAG::FLAG_INFO) {
			SetConsoleTextAttribute(HConsole_, FOREGROUND_BLUE | FOREGROUND_INTENSITY);

			if (!Timer::getSysMeasured())
				std::cout << "INFO: ";
		}
		else if (realFlag == FLAG::FLAG_WARNING){
			SetConsoleTextAttribute(HConsole_, FOREGROUND_RED | FOREGROUND_GREEN);

			if (!Timer::getSysMeasured())
				std::cout << "WARNING: ";
		}
		else if (realFlag == FLAG::FLAG_ERROR) {
			SetConsoleTextAttribute(HConsole_, FOREGROUND_RED | FOREGROUND_INTENSITY);

			if (!Timer::getSysMeasured())
				std::cout << "ERROR: ";
		}

		if (Timer::getSysMeasured())
			std::cout << "(" << Timer::getSysHMSStr() << ") ";

		std::cout << output << std::endl;

		// change colors back in case some1 wants to log using std::cout
		SetConsoleTextAttribute(HConsole_, defaultColors_);
	}
	sf::Vector2u Console::getSize() {
		RECT rect;
		HWND console = GetConsoleWindow();
		GetWindowRect(console, &rect); // stores the console's current dimensions

		return { static_cast<unsigned>(rect.right - rect.left),
				 static_cast<unsigned>(rect.bottom - rect.top) };
	}
	void Console::setSize(const sf::Vector2u& size)
	{
		RECT rect;
		HWND console = GetConsoleWindow();
		GetWindowRect(console, &rect); // stores the console's current dimensions

		MoveWindow(console, rect.left, rect.top, size.x, size.y, TRUE);
	}
	void Console::setFontStyle(const sf::Vector2i& size, const bool isBold)
	{
		CONSOLE_FONT_INFOEX font;
		font.cbSize = sizeof(font);

		font.dwFontSize.X = size.x;
		font.dwFontSize.Y = size.y;

		if(isBold)
			font.FontWeight = FW_BOLD;
		else 
			font.FontWeight = FW_NORMAL;

		SetCurrentConsoleFontEx(HConsole_, false, &font);
	}
	void Console::setName(const LPCSTR name)
	{
		SetConsoleTitle(name);
	}
	bool Console::isActive()
	{
		return IsWindowVisible(GetConsoleWindow());
	}
}