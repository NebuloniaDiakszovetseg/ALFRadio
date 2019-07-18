#pragma once
#include "NGin/Base.h" // resource loading
#include "NGin/UserInterface.h" // UI stuff

#include "bass.h" // mp3 player
#include "Input.h" // contains file names thet should be loaded

class MPlayer : public sf::Drawable {
public:
	void Setup();
	void handleEvents(const sf::Event& event);
	void Update(const HCHANNEL& channel);
	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
	/*Seeker*/
	void setSeekerPos(float pos) { seeker.setLevel(pos); }
	QWORD getSeekerPos() { return QWORD(ch_length * double(seeker.getLevel())); }
	QWORD getChannelLength() { return ch_length; }
	bool seekerMoved() { return seeker.getLevel() != ch_lenper; }

	/*Play/Pause*/
	void setPlayActive(bool isActive) { play.setisActive(isActive); }
	bool playMusic() { return play.getisActive() && ch_length != -1; }
	bool stopMusic() { return !play.getisActive() && ch_length != -1; }
private:
	/*Music Player*/
	sf::Sprite banner; // the banner above the player's controls
	NGin::UI::Slider seeker{ {475, 35}, {35, 35}, {25, 25} }; // the seek slider
	NGin::UI::Switcher play; // play button
	NGin::UI::Button prev{ {60, 60} };
	NGin::UI::Button next{ {60, 60} };

	QWORD ch_length = 0; // length of channel in bits
	float ch_lenper = 0.0f; // length of channel in percents
};