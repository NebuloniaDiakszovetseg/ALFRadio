#include "MPlayer.h"
#include "NGin/Base.h"

void MPlayer::Setup()
{
	banner.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("banner.png"));
	banner.setPosition({ 300,0 });

	play.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("play.png"));
	play.setPosition({ 650 - play.getSize().x / 2 , 415 });

	seeker.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("seeker.png"));
	seeker.setPosition({ 650 - seeker.getSize().x / 2 , 510 });

	prev.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("prev.png"));
	prev.setPosition({ 530, 415 });

	next.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("next.png"));
	next.setPosition({ 710, 415 });

	lengthText_.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	lengthText_.setCharacterSize(12);
	lengthText_.setPosition({ 885, 490 });

	positionText_.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	positionText_.setCharacterSize(12);
	positionText_.setPosition({ 380, 490 });

	seeker.setLevel(0);
}

void MPlayer::handleEvents(const sf::Event& event)
{
	next.select(NGin::UI::Cursor::getPosition());
	next.handleEvents(event);

	prev.select(NGin::UI::Cursor::getPosition());
	prev.handleEvents(event);

	play.select(NGin::UI::Cursor::getPosition());
	play.handleEvents(event);

	seeker.select(NGin::UI::Cursor::getPosition());
	seeker.handleEvents(event);

	// forward skip
	if (next.activated()) {
		Input::nextFile();
		seeker.setLevel(0);
	}
	// backward skip
	else if (prev.activated()) {
		Input::prevFile();
		seeker.setLevel(0);
	}
}

void MPlayer::Update(const HCHANNEL& channel)
{
	// make sure the saved channel length is right
	ch_length = BASS_ChannelGetLength(channel, BASS_POS_BYTE);

	// if the music is playing
	if (playMusic()) {
		// sets the seeker level to current length
		ch_lenper = float(BASS_ChannelGetPosition(channel, BASS_POS_BYTE)) / ch_length;
		seeker.setLevel(ch_lenper);
	}

	// Update length text
	double rawLength = BASS_ChannelBytes2Seconds(channel, ch_length);
	int lengthInMin = static_cast<int>(rawLength / 60);
	int lengthInSec = static_cast<int>(rawLength - (lengthInMin * 60));

	std::string lengthStr;

	if (lengthInMin < 10) lengthStr = "0" + std::to_string(lengthInMin) + ":";
	else lengthStr = std::to_string(lengthInMin) + ":";

	if (lengthInSec < 10) lengthStr += "0" + std::to_string(lengthInSec);
	else lengthStr += std::to_string(lengthInSec);

	lengthText_.setString(convertBytes(channel, ch_length));

	positionText_.setString(convertBytes(channel,
		BASS_ChannelGetPosition(channel, BASS_POS_BYTE)));
}

void MPlayer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(banner);
	target.draw(play);
	target.draw(seeker);
	target.draw(prev);
	target.draw(next);
	target.draw(lengthText_);
	target.draw(positionText_);
}

std::string MPlayer::convertBytes(const HCHANNEL& channel, const QWORD& bytes)
{
	double rawLength = BASS_ChannelBytes2Seconds(channel, bytes);
	int lengthInMin = static_cast<int>(rawLength / 60);
	int lengthInSec = static_cast<int>(rawLength - (lengthInMin * 60));

	std::string lengthStr = "";

	if (lengthInMin < 10) lengthStr = "0" + std::to_string(lengthInMin) + ":";
	else lengthStr = std::to_string(lengthInMin) + ":";

	if (lengthInSec < 10) lengthStr += "0" + std::to_string(lengthInSec);
	else lengthStr += std::to_string(lengthInSec);

	return lengthStr;
}
