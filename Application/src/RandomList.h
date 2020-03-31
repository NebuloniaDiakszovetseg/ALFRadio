#pragma once
#include "NGin.h"
#include <vector>
#include <string>

static constexpr auto RANDOM_LIST_FILE = "random_list.txt";

class RandomList {
public:
	static void load();
	static void update();

	// gets the location of a random file out of random_list.json
	// according to current index and settings.json content
	static std::string getFilePath(int i);

	static std::string getFolderPath();

	static bool noInput() {
		return paths_.size() == 0;
	}

	static void generateRandomPath() {
		pathIndex_ = rand() % paths_.size();
	}

	static bool isUsed() { return isUsed_; }
	static void markAsUsed() {
		usedOnDay = ng::Timer::getSysYMDStr();
		isUsed_ = true;
	}
private:
	static void create();

	static int pathIndex_;
	static std::vector<std::string> paths_;

	static bool isUsed_;
	static std::string usedOnDay;

	enum class INPUT_TYPE {
		OK = 0,
		EMPTY,
		CLEAR_FAIL
	};

	// cleans input from comments (//)
	// and empty lines
	static INPUT_TYPE cleanInput(std::string& input);
};