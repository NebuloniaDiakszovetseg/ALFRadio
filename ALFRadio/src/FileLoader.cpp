#include "FileLoader.h"

std::string FileLoader::getCurrFileString()
{
	return std::string("input/" + NGin::Timer::getSysDateString() + "/" + files[curr_index]);
}

bool FileLoader::indexUp()
{
	prev_index = curr_index;
	if (++curr_index < files.size()) {
		return true;
	}
	else {
		curr_index--;
		return false;
	}
}

bool FileLoader::indexDown()
{
	prev_index = curr_index;
	if (--curr_index >= 0) {
		return true;
	}
	else {
		curr_index++;
		return false;
	}
}
