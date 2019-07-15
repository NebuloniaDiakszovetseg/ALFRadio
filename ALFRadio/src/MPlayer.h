#pragma once
#include "NGin/Base.h" // resource loading
#include "NGin/UserInterface.h" // UI stuff

#include "bass.h" // mp3 player
#include "FileLoader.h" // loads in and manages necessary files
#include "InfoScreen.h" // displays information screen when needed

class MPlayer {
public:
	void Setup(sf::RenderWindow& window);
	void handleEvents(const sf::Event& event);
	void Update(sf::RenderWindow& window);
	void Compose(sf::RenderWindow& window);
private:
	// loads music with fileLoader.currFile()
	// neeeds window for error handling
	bool loadMusicFile();

	/*Cpanel*/
	sf::RectangleShape c_panel; // background color of the panel
	sf::Text date_time; // to display date

		/*Info*/
		sf::Sprite info_button; // nebulonia logo
		InfoScreen info_screen; // holds info screen graphics

		/*Volume*/
		sf::Text vol_text; // display text "volume" above corresponding slider
		NGin::UI::Slider volume{ {180, 35}, {35, 35}, {26, 25} }; // to adjust volume
		const float max_volume = 2.0f; // the maximum volume at which the slider stops
		float volume_val = 1.0f; // 0-none 1-normal max_volume-max

		/*Switches*/
		NGin::UI::Switcher inoutro{ {40, 40} }; // if active sticks the intro and outro to the beginning/end of each piece
		sf::Text inout_txt; // the text next to intro-outro switch

	/*Music Player*/
	sf::Sprite banner; // the banner above the player's controls
	NGin::UI::Slider seeker{ {475, 35}, {35, 35}, {25, 25} }; // the seek slider
	NGin::UI::Switcher play; // play button
	NGin::UI::Button prev{ {60, 60} };
	NGin::UI::Button next{ {60, 60} };

	/*File Loader*/
	FileLoader fileLoader;

	/*Actual Music*/
	HSAMPLE outro{}; // piece getting played after break
	HSAMPLE intro{};  // piece getting played before break
	// the music file that gets loaded into memory
	HSAMPLE sample{};
	// the channel through which the music gets played
	HCHANNEL channel{};
	// the length of the music currently playing (in bytes)
	QWORD channelLength = 0;
};