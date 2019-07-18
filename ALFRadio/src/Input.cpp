#include "Input.h"

int Input::prev_index = 0;
int Input::curr_index = 0;
std::vector<std::string> Input::file_names = {"1.szunet.mp3",
										 "2.szunet.mp3",
										 "3.szunet.mp3",
										 "4.szunet.mp3",
										 "5.szunet.mp3",
										 "6.szunet.mp3"};

std::vector<std::string> Input::start_times = { "08:20",
												"09:20",
												"10:20",
												"11:20",
												"12:30",
												"13:30" };

std::vector<std::string> Input::end_times = {"08:32",
											 "09:32",
											 "10:32",
											 "11:42",
											 "12:42",
											 "13:42" };

std::string Input::getCurrFileString()
{
	return std::string("input/" + NGin::Timer::getSysYMDStr() + "/" + file_names[curr_index]);
}

bool Input::nextFile()
{
	prev_index = curr_index;
	if (++curr_index < int(file_names.size())) {
		return true;
	}
	else {
		curr_index--;
		return false;
	}
}

bool Input::prevFile()
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

void Input::resetFile()
{
	curr_index = 0;
}
