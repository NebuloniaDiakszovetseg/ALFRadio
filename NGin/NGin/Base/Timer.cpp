#include "Timer.h"

namespace NGin {
	tm Timer::sys_time; // defaults all values to 0
	sf::Clock Timer::clock;
	float Timer::delta_time = 0;

	std::string Timer::year;
	std::string Timer::mon;
	std::string Timer::day;
	std::string Timer::hour;
	std::string Timer::min;
	std::string Timer::sec;

	void Timer::measureSysTime()
	{
		std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
		std::time_t start_time = std::chrono::system_clock::to_time_t(now);
		errno_t err = localtime_s(&sys_time, &start_time);

		/*converts time numbers into calendaristic-format*/
		sys_time.tm_year += 1900;
		sys_time.tm_mon += 1;

		year = std::to_string(sys_time.tm_year);

		if (sys_time.tm_mon < 10) mon = "0" + std::to_string(sys_time.tm_mon);
		else mon = std::to_string(sys_time.tm_mon);

		if (sys_time.tm_mday < 10) day = "0" + std::to_string(sys_time.tm_mday);
		else day = std::to_string(sys_time.tm_mday);

		if (sys_time.tm_hour < 10) hour = "0" + std::to_string(sys_time.tm_hour);
		else hour = std::to_string(sys_time.tm_hour);
		
		if (sys_time.tm_min < 10) min = "0" + std::to_string(sys_time.tm_min);
		else min = std::to_string(sys_time.tm_min);

		if (sys_time.tm_sec < 10) sec = "0" + std::to_string(sys_time.tm_sec);
		else sec = std::to_string(sys_time.tm_sec);
	}
	void Timer::measureDeltaTime()
	{
		// Resets clock and times each frame
		delta_time = clock.getElapsedTime().asSeconds();
		clock.restart();
	}
}
