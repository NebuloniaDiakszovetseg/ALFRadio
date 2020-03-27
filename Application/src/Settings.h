#pragma once
#include "NGin.h"

static constexpr auto WIN_NAME = "Aprily Radio - RenderWindow";
static constexpr auto CON_NAME = "Aprily Radio - Console";

static constexpr int VIEW_WIDTH = 1920;
static constexpr int VIEW_HEIGHT = 1080;

// location of input files
static constexpr auto INPUT_LOC = "input/";

class Settings : public sf::Drawable
{
	// --- File I/O ----------------
public:
	// reads settings from settings.json
	static void load();
	// saves changes to settings.json
	static void save();

	static unsigned getWindowWidth() { return jsonFile[renderWindowKey][widthKey].get<unsigned>(); }
	static unsigned getWindowHeight() { return jsonFile[renderWindowKey][heightKey].get<unsigned>(); }
	static int getWindowType() { return jsonFile[renderWindowKey][typeKey].get<int>(); }
	static unsigned getConsoleWidth() { return jsonFile[consoleWindowKey][widthKey].get<unsigned>(); }
	static unsigned getConsoleHeight() { return jsonFile[consoleWindowKey][heightKey].get<unsigned>(); }
	static float getMaximumVolume() { return jsonFile[appVolumeKey].get<float>(); }
	static float getDimThreshold() { return jsonFile[dimThresholdKey].get<float>(); }
	static float getDimVolume() { return jsonFile[dimVolumeKey].get<float>(); }
	static float getDimDuration() { return jsonFile[dimDurationKey].get<float>(); }
	static std::string getIntroLocation() { return jsonFile[introLocKey].get<std::string>(); }
	static std::string getOutroLocation() { return jsonFile[outroLocKey].get<std::string>(); }
	static bool getAutoIntroOutro() { return jsonFile[autoInOutroKey].get<bool>(); }
	static bool getDimIsActive() { return jsonFile[dimActiveKey].get<bool>(); }
	static bool getConsoleIsDisabled() { return jsonFile[consoleWindowKey][isDisabled].get<bool>(); }

	// get the string that represents the start of break with index i (format -> hh:mm)
	static std::string getBreakStartString(int i) { return jsonFile[std::to_string(i)][startKey]; }
	// get the string that represents the end of break with index i (format -> hh:mm)
	static std::string getBreakEndString(int i) { return jsonFile[std::to_string(i)][endKey]; }
	// get the string that represents the name of file with index i
	static std::string getFileName(int i) { return jsonFile[std::to_string(i)][fileKey]; }

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

	static unsigned getNumOfFiles() { return numberOfFiles; }

	static void setAutoIntroOutro(bool isActive) { jsonFile[autoInOutroKey] = isActive; }
	static void setDimIsActive(bool isActive) { jsonFile[dimActiveKey] = isActive; }
private:
	// variables
	static ng::json jsonFile;
	static unsigned numberOfFiles; // number of files in one folder(szunetek szama)

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

	// --- User Interface -----------
public:
	void setup();
	void handleEvents(const sf::Event& event);
	void update();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	// --- Background ----------------------------
	const sf::Vector2f backgroundPosition_{ 600.0F, 0.0F };
	const sf::Vector2f backgroundSize_
	{ static_cast<float>(VIEW_WIDTH) - backgroundPosition_.x, 700.0F };
	sf::RectangleShape background_;
	const sf::Color backgroundColor_{ 30, 30, 30 };
	sf::RectangleShape banner_;
	const std::string bannerTextureLoc_ = "banner.png";

	// -- Settings Switcher ----------------------
	ng::Switcher settingsSwitcher_{ { 60.0F, 60.0F } };
	const sf::Vector2f settingsSwitcherPos_
	{ static_cast<float>(VIEW_WIDTH) - 80.0F,  20.0F };
	const sf::Vector2f settingsSwitcherSize_{ 60.0F, 60.0F };
	const std::string settingsTextureLoc_ = "settings_switcher.png";

	// -- Confirm Dialog -------------------------
	const sf::Color confirmDialogBgColor_ = { 60, 60, 60 };
	const float confirmDialogTextYOffset_ = -30.0F;
	const sf::Vector2f confirmDialogButtonScale_{ 0.75F, 1.0F };
	const sf::Vector2f confirmDialogButtonOffset_ = { 40.0F, 30.0F };

	const sf::Vector2f confirmDialogSize_{ 875.0F, 250.0F };
	const std::string confirmDialogString_ = 
	"Are you sure you want to keep settings this way ?"
	"\n                                 Reverting in : ";

	ng::ConfirmDialog confirmDialog_{
		ng::ConfirmDialog::DIALOG_TYPE::DIALOG_YES_OR_CLOSE,
		confirmDialogString_,
		confirmDialogSize_,
	     {backgroundPosition_.x + backgroundSize_.x / 2.0F - confirmDialogSize_.x / 2.0F,
		  backgroundPosition_.y + backgroundSize_.y / 2.0F - confirmDialogSize_.y / 2.0F },
	};

	// --- Apply Button -------------------
	ng::Button applyButton_{ "Change resolution", {400, 50} };
	const sf::Vector2f applyButtonPosition_{ static_cast<float>(VIEW_WIDTH) - 500.0F, 610.0F };
	const sf::Vector2f applyButtonScale_{ 1.0F, 1.0F };

	// --- Window Resolutions Dropdown -------------------
	ng::Dropdown winResDropdown_;
	const sf::Vector2f winResDropdownPos_ = { 700.0F, 110.0F };
	sf::Vector2u saveWinRes_;
	std::vector<sf::Vector2u> resolutions_ =
		{ { 1920, 1080 },
		{ 1366, 768 },
		{ 1360, 768 },
		{ 1280, 800 },
		{ 1024, 576 },
		{ 800, 480 } }; // non-compatible resolutions will be filtered out

	sf::Text winResText_; // window resolutions text
	const std::string winResString_ = "Window resolution:";

	// -- Window Type Dropdown ----------------------
	ng::Dropdown windowTypeDropdown_;
	const sf::Vector2f windowTypeDropdownPos_ = { 1340.0F, 110.0F };
	std::string saveWindowType_;
	const int fullScreenIndex_ = 2; // should point at windowTypes' full screen
	const std::vector<std::string> windowTypes_{
		"Windowed",
		"Borderless",
		"Fullscreen"
	};

	sf::Text windowTypeText_;
	const std::string windowTypeString_ = "Window Type:";

	// --- Console Resolutions Dropdown -------------------
	ng::Dropdown conResDropdown_;
	const sf::Vector2f conResDropdownPos_ = { 700.0F, 285.0F };
	sf::Vector2u saveConRes_;

	sf::Text conResText_; // window resolutions text
	const std::string conResString_ = "Console resolution:";

	// --- Max Volume Slider ------------------------
	sf::Text maxVolumeText_;
	const std::string maxVolumeString_ = "Maximum volume: "; // % gets added in program

	ng::Slider maxVolumeSlider_;
	const sf::Vector2f maxVolumeSliderPosition_ = { 1300.0F, 285.0F };

	// --- Dim Threshold Slider ------------------------
	sf::Text dimThresholdText_;
	const std::string dimThresholdString_ = "Dim threshold: "; // % gets added in program

	ng::Slider dimThresholdSlider_;
	const sf::Vector2f dimThresholdSliderPosition_ = { 660.0F, 470.0F };

	sf::RectangleShape micValueRect_;
	sf::RectangleShape micMaxValueRect_;
	const sf::Vector2f micMaxValueRectPos_ = { 710.0F, 550.0F };
	const sf::Vector2f micMaxValueRectSize_ = { 375.0F, 30.0F };

	// --- Dim Volume Slider ------------------------
	sf::Text dimVolumeText_;
	const std::string dimVolumeString_ = "Dim volume: "; // % gets added in program

	ng::Slider dimVolumeSlider_;
	const sf::Vector2f dimVolumeSliderPosition_ = { 1300.0F, 470.0F };

	// --- General Properties -----------------------
	const std::string sliderLocation_ = "volume_slider.png";
	const float sliderScale_ = 1.25F;
	const float sliderSelectThickness_ = -3.0F;

	const std::string fontTypeLocation_ = "keep_calm_font.ttf";
	const sf::Color fontColor_{ 243, 96, 0 };
	const unsigned fontSize_ = 30;
	const float textYOffset_ = -55.0F;

	const std::string buttonTextureLoc_ = "button.png";

	const std::string dropdownTextureLoc_ = "dropdown.png";
	const sf::Vector2f dropdownScale_ = { ng::ftovec(1.3F) };

	const sf::Color disabledElementColor_{ 0, 0, 0 };

	float resetTimer_ = 0.0F;
	const float resetLimit_ = 10.0F;

	enum class CHANGE {
		CHANGE_NONE,
		CHANGE_TEMPORARY,
		CHANGE_PERMANENT,
		CHANGE_REVERT
	};
	void applyNewSettings(const CHANGE changeType);
};