#pragma once
#include "NGin.h"

static constexpr auto WIN_NAME = "Aprily Radio - RenderWindow";
static constexpr auto CON_NAME = "Aprily Radio - Console";

static constexpr int VIEW_WIDTH = 1920;
static constexpr int VIEW_HEIGHT = 1080;

// location of input files
static constexpr auto INPUT_LOC = "input/";

class Settings
{
	// --- File I/O ----------------
public:
	// reads settings from settings.json
	static void load();
	// saves changes to settings.json
	static void save();

	static unsigned getWindowWidth() { return jsonFile_[renderWindowKey][widthKey].get<unsigned>(); }
	static unsigned getWindowHeight() { return jsonFile_[renderWindowKey][heightKey].get<unsigned>(); }
	static int getWindowType() { return jsonFile_[renderWindowKey][typeKey].get<int>(); }
	static unsigned getConsoleWidth() { return jsonFile_[consoleWindowKey][widthKey].get<unsigned>(); }
	static unsigned getConsoleHeight() { return jsonFile_[consoleWindowKey][heightKey].get<unsigned>(); }
	static float getMaximumVolume() { return jsonFile_[appVolumeKey].get<float>(); }
	static float getDimThreshold() { return jsonFile_[dimThresholdKey].get<float>(); }
	static float getDimVolume() { return jsonFile_[dimVolumeKey].get<float>(); }
	static float getDimDuration() { return jsonFile_[dimDurationKey].get<float>(); }
	static std::string getIntroLocation() { return jsonFile_[introLocKey].get<std::string>(); }
	static std::string getOutroLocation() { return jsonFile_[outroLocKey].get<std::string>(); }
	static bool getAutoIntroOutro() { return jsonFile_[autoInOutroKey].get<bool>(); }
	static bool getDimIsActive() { return jsonFile_[dimActiveKey].get<bool>(); }
	static bool getConsoleIsDisabled() { return jsonFile_[consoleWindowKey][isDisabled].get<bool>(); }

	// get the string that represents the start of break with index i (format -> hh:mm)
	static std::string getBreakStartString(int i) { return jsonFile_[std::to_string(i)][startKey]; }
	// get the string that represents the end of break with index i (format -> hh:mm)
	static std::string getBreakEndString(int i) { return jsonFile_[std::to_string(i)][endKey]; }
	// get the string that represents the name of file with index i
	static std::string getFileName(int i) { return jsonFile_[std::to_string(i)][fileKey]; }

	static int getBreakStartHour(int i) {
		return std::stoi(getBreakStartString(i));
	}
	static int getBreakEndHour(int i) {
		return std::stoi(getBreakEndString(i));
	}
	static int getBreakStartMinute(int i) {
		std::size_t pos = getBreakStartString(i).find(":");  // position of ":" in str
		return std::stoi(getBreakStartString(i).substr(pos + 1)); // get from ":" to the end
	}
	static int getBreakEndMinute(int i) {
		std::size_t pos = getBreakEndString(i).find(":");  // position of ":" in str
		return std::stoi(getBreakEndString(i).substr(pos + 1)); // get from ":" to the end
	}

	// returns the relative path of file with index i
	static std::string getFilePath(int i) {
		return INPUT_LOC + ng::Timer::getSysYMDStr() + "/" + Settings::getFileName(i);
	}

	static unsigned getNumOfFiles() { return numberOfFiles_; }

	static void setAutoIntroOutro(bool isActive) { jsonFile_[autoInOutroKey] = isActive; }
	static void setDimIsActive(bool isActive) { jsonFile_[dimActiveKey] = isActive; }

protected:  // accessed by settings UI
	// variables
	static ng::json jsonFile_;
	static unsigned numberOfFiles_; // number of files in one folder(szunetek szama)

	// keys
#define KEY static constexpr const char*
	KEY renderWindowKey = "render_window";
	KEY consoleWindowKey = "console_window";
	KEY isDisabled = "is_disabled";
	KEY widthKey = "width";
	KEY heightKey = "height";
	KEY typeKey = "window_type";
	KEY appVolumeKey = "maximum_app_volume";
	KEY startKey = "1.start";
	KEY endKey = "2.end";
	KEY fileKey = "file";
	KEY autoInOutroKey = "auto_intro_and_outro";
	KEY introLocKey = "intro_location";
	KEY outroLocKey = "outro_location";
	KEY dimActiveKey = "dim_is_active";
	KEY dimThresholdKey = "dim_threshold";
	KEY dimVolumeKey = "dim_volume";
	KEY dimDurationKey = "dim_duration_seconds";
#undef KEY
};