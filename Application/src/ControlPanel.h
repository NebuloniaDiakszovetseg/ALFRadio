#pragma once
#include "NGin.h"
#include "Settings.h"
#include "Elements/Table.h"

static constexpr float DEFAULT_VOLUME_VALUE = 0.50F;

class ControlPanel : public sf::Drawable {
public:
	void setup();
	void handleEvents(const sf::Event& event);
	void update();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
private:
	// --- Background -----------------
	const sf::Vector2f backgroundPosition_{ 0.0F, 0.0F };
	const sf::Vector2f backgroundSize_{ 600.0F, static_cast<float>(VIEW_HEIGHT) };
	sf::RectangleShape background_;
	const sf::Color backgroundColor_{ 60, 60, 60 };

	// --- Information Screen ---------
	sf::Sprite infoScreen_;
	sf::Sprite settingsInfoScreen_;
	const std::string infoScreenTextureLoc_ = "info_screen.png";
	const std::string settingsInfoScreenTextureLoc_ = "info_screen_settings.png";

	sf::RectangleShape infoHoverOn_;
	const sf::Vector2f infoSize_{ 125.0F, 125.0F };
	const sf::Vector2f infoPosition_{ 35.0F, 22.0F };
	const std::string infoTexture1Location_ = "info.png";
	const std::string infoTexture2Location_ = "nebulogo.png";
	bool infoToggle_ = false;

	// -- Date And Time Display -------
	sf::Text dateTime_; // displays date and time on RenderWindow
	const sf::Vector2f dateTimePos_{ 190.0F, 63.0F }; // position of date and time

	// -- Volume Text And Slider ------
	sf::Text volumeText_;
	const float volumeTextOffsetY_ = -355.0F; // position offset of volumeText on Y axis
	const std::string volumeString_ = "Volume: ";  // % gets added in program

	ng::Slider volumeSlider_;
	const std::string volumeSliderLoc_ = "volume_slider.png";
	const float volumeSliderPositionY_ = 225.0F;
	const float volumeSliderScale_ = 1.35F;
	const float volumeSliderSelectThickness_ = -3.0F;

	// -- Auto Intro-Outro -------------
	ng::Switcher introSwitcher_; // if active auto intro and outro get played
	const sf::Vector2f introSwitcherPosition_{ 45, 310 };
	sf::Text introSwitcherText_;
	const sf::Vector2f introSwitcherTextPos_{ 145, 322 };
	const std::string introSwitcherString_ = "Auto intro/outro";

	// -- Randomize not found files ----
	ng::Switcher notFoundSwitcher_;
	const sf::Vector2f notFoundSwitcherPosition_{ 45, 410 };
	sf::Text notFoundSwitcherText_;
	const sf::Vector2f notFoundSwitcherTextPos_{ 145, 422 };
	const std::string notFoundSwitcherString_ = "Randomize if F.N.F.";

	// -- Dim if microphone active ----
	ng::Switcher dimSwitcher_;
	const sf::Vector2f dimSwitcherPosition_{ 45, 510 };
	sf::Text dimSwitcherText_;
	const sf::Vector2f dimSwitcherTextPos_{ 145, 522 };
	const std::string dimSwitcherString_ = "Dim if MIC. active";

	float dimTimer_;
	float volumeBackup_ = - 1;

	// -- Table of Files ---------------
	Table table_{ { 530 , 460 }, { 35, 600 } };
	const float tableRefreshRate_ = 1.0F;

	// -- General Properties -----------
	const std::string fontTypeLocation_ = "keep_calm_font.ttf";
	const sf::Color fontColor_{ 243, 96, 0 };
	const unsigned fontSize_ = 34;

	const float switcherScale_ = 1.2F;
	const std::string switcherTextureLocation_ = "switcher.png";
	const sf::Color switcherMarkColor_{ 0, 0, 0 };
	const sf::Color elementColor_{ 250, 130, 52 };
	const sf::Color disabledElementColor_{ 0, 0, 0 };
};
