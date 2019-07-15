#pragma once
#include "NGin/Base.h"
#include "NGin/UserInterface.h"

#include "MPlayer.h"

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
	void run(sf::RenderWindow& window);
private:
	MPlayer mPlayer;
};