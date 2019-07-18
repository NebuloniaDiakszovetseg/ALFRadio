#pragma once
#include "SFML/System.hpp" // for sf::Clock

// to measure time
#include <chrono>
#include <ctime>

namespace NGin {
	class Timer {
	public:
		static void measureSysTime();
		static void measureDeltaTime();

		// returns true if system time was measured
		static bool getSysMeasured() { return sys_time.tm_year; }
		// get system time as tm object
		static tm getSys() { return sys_time; }
		// get system time in yyyy.mm.dd hh format as string
		static std::string getSysString() { return year + "." + mon + "." + day + " " + hour + ":" + min + ":" + sec; }
		// get system date in yyyy.mm.dd format as string
		static std::string getSysYMDStr() { return year + "." + mon + "." + day; }
		// get system time in h::mm:ss format as string
		static std::string getSysHMSStr() { return hour + ":" + min + ":" + sec; }
		// get system time in hh:mm format as string
		static std::string getSysHMStr() { return hour + ":" + min; }

		// get delta the delta time measured
		static float getDeltaTime() { return delta_time; }

	private:
		static tm sys_time; // the system date and time at running

		// hold each important tm time value in a string
		static std::string year;
		static std::string mon;
		static std::string day;
		static std::string hour;
		static std::string min;
		static std::string sec;

		static sf::Clock clock; // measures application runtime
		static float delta_time; // delta time value for each frame
	};
}
