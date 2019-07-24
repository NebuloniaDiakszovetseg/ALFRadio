#pragma once
#include "NGin/Base.h"
#include "NGin/UserInterface.h"

#include "Input.h" // contains file names thet should be loaded
#include "RandomInput.h"
#include "MPlayer.h"
#include "CPanel.h"

// render window properties
static constexpr auto win_name = "Aprily Radio - RenderWindow";
static constexpr int w_width = 1000;
static constexpr int w_height = 600;

// console window properties
static constexpr auto con_name = "Aprily Radio - Console";
static constexpr int c_width = 800;
static constexpr int c_height = 480;

class Application {
public:
	void Setup();
	void handleEvents(const sf::Event& event);
	void Update(sf::RenderWindow& window);
	void Compose(sf::RenderWindow& window);

	void loadCurrInput();

	// the volume level of microphone in percentage
	static float microphoneLevel;
private:
	MPlayer mPlayer; // music player
	CPanel cPanel; // control panel
	
	// saves today's date to check on it if it changes
	int today;

	float dimTimer_ = 0.0f;
	float volumeBackup_ = 1.0f;
	bool volumeDimmed_ = false;

	/*Actual Music*/
	// the music file that gets loaded into memory
	HSAMPLE sample{};
	// the channel through which the music gets played
	HCHANNEL channel{};
	// length of the current sample in bits
	QWORD curr_sample_length = 0;
};