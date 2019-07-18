#pragma once
#include <vector>
#include <string>

#include "NGin/Base.h"

class Input {
public:
	// gets the current files's name
	static std::string getFileName(const int index) { return file_names[index]; }
	//gets the number of files
	static unsigned nrofFiles() { return file_names.size(); }
	// gets the current file's entrie relative path
	static std::string getCurrFileString();
	// signals whenever file index changes
	static bool hasChanged() { return prev_index != curr_index; }
	// this should be called whenever input gets loaded
	static void isLoaded() { prev_index = curr_index; }
	static int getCurrIndex() { return curr_index; }

	static std::string getStartTime(int index) { return start_times[index]; }
	static std::string getEndTime(int index) { return end_times[index]; }

	// adds 1 to file index - returns false if fail
	static bool nextFile();
	// substracts 1 from file index - returns false if fail
	static bool prevFile();
	// resets Input to first file
	static void resetFile();
private:
	// holds the last loaded index
	static int prev_index; // has changed = false by default

	// the index of the current file
	static int curr_index;
	
	// the names of individual files
	static std::vector<std::string> file_names;
	static std::vector<std::string> start_times;
	static std::vector<std::string> end_times;
};