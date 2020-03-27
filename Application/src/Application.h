#pragma once
#define MAIN_LEVEL
#include "NGin.h"
#include "bass.h"

#include "BassPlayer.h"
#include "Settings.h"
#include "MusicPlayer.h"
#include "ControlPanel.h"

// app created by: Szoke Andras-Lorand
// <add your name>

class Application : public ng::Main
{
public:
	Application();
	~Application();
	
	/* abstract overrides */
	virtual void handleEvents();
	virtual void update();
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	const std::string windowIconString_ = "icon.png";
	const unsigned frameRateLimit_ = 60;

	Settings settings_;
	ControlPanel controlPanel_;
	MusicPlayer musicPlayer_;
};

ng::Main* setMainLevel() {
	return new Application;
}