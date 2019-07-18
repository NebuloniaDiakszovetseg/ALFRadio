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
			table_elem.push_back(sf::RectangleShape{});
			table_txt.push_back(sf::Text{});
		}
	}

	float getVolume() { return volume_val; }
	void playIntro(HCHANNEL& channel);
	void playOutro(HCHANNEL& channel);
	bool inoutActive() { return inoutro.getisActive(); }
private:
	/*Unmoving elements*/
	sf::RectangleShape background; // background color of the panel
	sf::Text date_time; // to display date

	/*Info*/
	sf::Sprite info_button; // nebulonia logo
	sf::Sprite info_screen; // overlay showing informations when activated

	/*Volume*/
	sf::Text vol_text; // display text "volume" above corresponding slider
	NGin::UI::Slider volume{ {180, 35}, {35, 35}, {26, 25} }; // to adjust volume
	const float max_volume = 2.0f; // the maximum volume at which the slider stops
	float volume_val = 1.0f; // 0-none 1-normal max_volume-max

	/*Switches*/
	NGin::UI::Switcher inoutro{ {40, 40} }; // if active sticks the intro and outro to the beginning/end of each piece
	sf::Text inout_txt; // the text next to intro-outro switch

	/*File-table*/
	sf::RectangleShape header_elem;
	sf::Text header_txt;
	std::vector<sf::RectangleShape> table_elem;
	std::vector<sf::Text> table_txt;

	HSAMPLE intro{};  // piece getting played before break
	QWORD intro_length = 0; // the length of the intro in bytes
	HSAMPLE outro{}; // piece getting played after break
	QWORD outro_length = 0; // the length of the outro in bytes
private:
	void loadInOut();
};