#pragma once
#include <vector>
#include <string>

#include "NGin/Base.h"

class FileLoader {
public:
	std::string getFileName(const int index) const { return files[index]; }
	std::string getCurrFileString();
	bool hasChanged() { return prev_index != curr_index; }

	// adds 1 to file index - returns false if fail
	bool indexUp();
	// substracts 1 from file index - returns false if fail
	bool indexDown();
private:
	// holds the last loaded index
	int prev_index = -1; // has changed = false by default

	// the index of the current file
	int curr_index = 0;

	std::vector<std::string> files = {
		"1.szunet.mp3",
		"2.szunet.mp3",
		"3.szunet.mp3",
		"4.szunet.mp3",
		"5.szunet.mp3",
		"6.szunet.mp3"
	};
};