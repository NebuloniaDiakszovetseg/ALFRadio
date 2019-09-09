#pragma once
#include "NGin/Base.h" // resource loading
#include "NGin/UserInterface.h" // UI stuff

#include "bass.h" // mp3 player
#include "Input.h"

class CPanel : public sf::Drawable {
public:
	void Setup();
	void handleEvents(const sf::Event& event);
	void Update();
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
	CPanel() {
		for (int i = 0; i < int(Input::nrofFiles()); i++) {
			tableShape_.push_back(sf::RectangleShape{});
			tableText_.push_back(sf::Text{});
		}

		infoScreen_.setColor(sf::Color::Transparent);
		headerText_.setString("Nothing loaded yet");
	}

	void setHeaderText(std::string headerText) {
		headerText_.setString(headerText);
		NGin::offCentTxtInSh(10, headerText_, headerShape_);
	}
	void randomSetActive(const bool active) { randomSwitcher_.setisActive(active); }
	void setVolume(const float volumeValue) { volumeSlider_.setLevel(volumeValue / maxVolume_);  volumeValue_ = volumeValue; }

	float getVolume() { return volumeValue_; }
	void playIntro(HCHANNEL& channel);
	bool introStopped(const HCHANNEL& channel);
	void playOutro(HCHANNEL& channel);
	bool outroStopped(const HCHANNEL& channel);
	bool introShouldPlay() { return introSwitcher_.isActive(); }
	bool randomGetActive() { return randomSwitcher_.isActive(); }
	bool dimOnMicGetActive() { return dimOnMic_.isActive(); }
private:
	/*Unmoving elements*/
	sf::RectangleShape background_; // background color of the panel
	sf::Text dateTime_; // to display date

	/*Info*/
	sf::Sprite infoButton_; // nebulonia logo
	sf::Sprite infoScreen_; // overlay showing informations when activated

	/*Volume*/
	sf::Text volumeText_; // display text "volume" above corresponding slider
	NGin::UI::Slider volumeSlider_{ {180, 35}, {35, 35}, {26, 25} }; // to adjust volume
	const float maxVolume_ = 1.5f; // the maximum volume at which the slider stops
	float volumeValue_ = 1.0f; // 0-none 1-normal max_volume-max

	// Auto Intr-Outro
	NGin::UI::Switcher introSwitcher_{ {40, 40} }; // activates intro AND outro
	sf::Text introText_; // the text next to intro-outro switch

	// Random NF Files
	NGin::UI::Switcher randomSwitcher_{ {40, 40} }; // if active plays random songs when file not found
	sf::Text randomText_; // the text next to intro-outro switch

	NGin::UI::Switcher dimOnMic_{{ 40, 40 }}; // dims channel volume when mic has input
	sf::Text dimOnMicText_; // the text next to dimmer switch

	/*File-table*/
	sf::RectangleShape headerShape_;
	sf::Text headerText_;
	std::vector<sf::RectangleShape> tableShape_;
	std::vector<sf::Text> tableText_;

	HSAMPLE introSample_{};  // piece getting played before break
	QWORD introLength_ = 0; // the length of the intro in bytes
	HSAMPLE outroSample_{}; // piece gtting played after break
	QWORD outroLength_ = 0; // the length of the outro in bytes
private:
	void loadInOut();
};