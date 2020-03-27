#pragma once
#include "NGin.h"
#include "Settings.h"

class MusicPlayer : public sf::Drawable {
public:
	void setup();
	void handleEvents(const sf::Event& event);
	void update();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	// prevents continuos starting of the same file over ad over
	std::string startedForTime = "";
	std::string endedForTime = "";

	// --- Background -----------------
	const sf::Vector2f backgroundPosition_{ 600.0F, 700.0F };
	const sf::Vector2f backgroundSize_{
		static_cast<float>(VIEW_WIDTH) - backgroundPosition_.x,
		static_cast<float>(VIEW_HEIGHT) - backgroundPosition_.y };
	sf::RectangleShape background_;
	const sf::Color backgroundColor_{ 0, 0, 0 };

	// -- User Interface --------------
	const float controlsYPos_ = 770;
	const float controlsXOffset_ = 60;
	const float controlsScale_ = 1.7F;

	ng::Switcher playSwitcher_;
	const std::string playSwitcherTextureLoc_ = "play_switcher.png";
	ng::Button nextButton_{ { 60, 60 } };
	const std::string nextSwitcherTextureLoc_ = "next_button.png";
	ng::Button prevButton_{ { 60, 60 } };
	const std::string prevSwitcherTextureLoc_ = "prev_button.png";

	ng::Slider seekSlider_{ {475, 35}, {35, 35}, {25, 25} };
	const float seekSliderYPos_ = 940;
	const float seekSliderScale_ = 1.95F;
	const std::string seekSliderTextureLoc_ = "seek_slider.png";

	const sf::Vector2f timeTextOffset_ = { 10, 30 }; // relative to slider (abs values)
	sf::Text currentTime_; // current time stamp
	sf::Text totalTime_; // static text gives total length of file
	const unsigned fontSize_ = 28;
	std::string fontTypeLoc_ = "arial_font.ttf";
	sf::Color fontColor_ = sf::Color::White;
};