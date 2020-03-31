#include "RandomList.h"

#include <fstream>
#include <stdlib.h>
#include <time.h>

#include "NGin.h"
#include "Settings.h"

int RandomList::pathIndex_ = 0;
std::vector<std::string> RandomList::paths_;

bool RandomList::isUsed_;
std::string RandomList::usedOnDay;

void RandomList::load()
{
	/* initialize random seed: */
	srand(time(NULL));

	std::ifstream in(RANDOM_LIST_FILE);

	if (in.fail()) // if failed to load
	{
		create();
	}
	else {
		std::string input;

		while (std::getline(in, input)) {
			if (cleanInput(input) == INPUT_TYPE::OK) {
				paths_.push_back(input);
			}
		}
	}
}

void RandomList::update()
{
	if (usedOnDay != ng::Timer::getSysYMDStr()) {
		isUsed_ = false;
	}
}

std::string RandomList::getFilePath(int i)
{
	return paths_[pathIndex_] + Settings::getFileName(i);
}

std::string RandomList::getFolderPath()
{
	return paths_[pathIndex_];
}

void RandomList::create()
{
	// generate empty file with comments
	std::ofstream out(RANDOM_LIST_FILE);

	out << "// Add folder paths here to make them available"
		" for randomization by the app!\n";
	out << "// Example of a path: input/2020.03.31/\n";
	out << "// Make sure to add slashes properly!\n";
	out << "// Add each path to a new line!\n";

	out.close();

	NG_LOG_INFO("Random List created/saved successfully!");
}

RandomList::INPUT_TYPE RandomList::cleanInput(std::string& input)
{
	bool onlyspaces = true;

	size_t commentpos = input.find("//"); // these are the caracters that mean comment
	bool comment = false;

	// if comment found
	if (commentpos != std::string::npos) {
		input.erase(commentpos, input.length()); // erase comment
		comment = true;
	}

	// checks for potential empty lines and ignores them
	for (std::string::const_iterator i = input.begin(); i != input.end(); ++i)
	{
		if (onlyspaces && *i != ' ' && *i != '\t')
		{
			onlyspaces = false;
		}
	}

	if (input != " " && !onlyspaces) {
		return INPUT_TYPE::OK;
	}
	else if (!comment) {
		return INPUT_TYPE::CLEAR_FAIL;
	}
	else {
		return INPUT_TYPE::EMPTY;
	}
}
