#include "MPlayer.h"
#include "NGin/Base.h"

void MPlayer::Setup(sf::RenderWindow& window)
{
	banner.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("banner.png"));
	banner.setPosition({300,0});

	play.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("play.png"));
	play.setPosition({ 650 - play.getSize().x / 2 , 415});

	seeker.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("seeker.png"));
	seeker.setPosition({ 650 - seeker.getSize().x / 2 , 510 });

	prev.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("prev.png"));
	prev.setPosition({ 530, 415 });

	next.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("next.png"));
	next.setPosition({ 710, 415 });

	c_panel.setPosition({ 0, 0 });
	c_panel.setSize({ 300, 600 });
	c_panel.setFillColor(sf::Color(60, 60, 60));

	info_button.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("info.png"));
	info_button.setPosition({ 20, 20 });
	info_button.setScale({ 0.1f, 0.1f });

	vol_text.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	vol_text.setCharacterSize(16);
	vol_text.setFillColor(sf::Color(243, 96, 0));
	vol_text.setString("Volume:");
	vol_text.setPosition({ 150 - vol_text.getGlobalBounds().width / 2, 95 });

	volume.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("volume.png"));
	volume.setPosition({ 25, 120 });
	volume.setFillColor(sf::Color(250, 130, 52));

	inoutro.setPosition({ 25, 170 });
	inoutro.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("switcher.png"));
	inoutro.setFillColor(sf::Color(250, 130, 52));
	inoutro.setMarkColor(sf::Color(20, 20, 20));

	inout_txt.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	inout_txt.setFillColor(sf::Color(243, 96, 0));
	inout_txt.setString("Auto Intro/Outro");
	inout_txt.setCharacterSize(18);
	inout_txt.setPosition({ 75, 170 + inout_txt.getGlobalBounds().height / 2 + inout_txt.getLocalBounds().top });

	date_time.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	date_time.setPosition({ 90, 38 });
	date_time.setCharacterSize(18);
	date_time.setFillColor(sf::Color(243, 96, 0));


	intro = BASS_SampleLoad(false, "input/intro.mp3", 0, 0, 1, BASS_DEVICE_STEREO);
	if (BASS_ErrorGetCode() != BASS_OK) {
		NGin::Logger::log("Cannot open intro! - Auto intro/outro DISABLED!", NGin::Logger::Severity::Warning);
		inoutro.setisActive(false);
		inoutro.setInactivity(true);
	}
	else {
		outro = BASS_SampleLoad(false, "input/outro.mp3", 0, 0, 1, BASS_DEVICE_STEREO);
		if (BASS_ErrorGetCode() != BASS_OK) {
			NGin::Logger::log("Cannot open outro! - Auto intro/outro DISABLED!", NGin::Logger::Severity::Warning);
			inoutro.setisActive(false);
			inoutro.setInactivity(true);
		}
		else {
			NGin::Logger::log("Intro/Outro loaded successfully!");
		}
	}

	// closes window if loadMusicFile() fails
	if (!loadMusicFile()) window.close();

	// sets the volume up
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, volume_val);
	volume.setLevel(volume_val / max_volume);
}

void MPlayer::handleEvents(const sf::Event& event)
{
	volume.select(NGin::UI::Cursor::getPosition());
	volume.handleEvents(event);

	next.select(NGin::UI::Cursor::getPosition());
	next.handleEvents(event);

	prev.select(NGin::UI::Cursor::getPosition());
	prev.handleEvents(event);

	play.select(NGin::UI::Cursor::getPosition());
	play.handleEvents(event);

	seeker.select(NGin::UI::Cursor::getPosition());
	seeker.handleEvents(event);

	inoutro.select(NGin::UI::Cursor::getPosition());
	inoutro.handleEvents(event);

	// info-screen toggle
	if (info_button.getGlobalBounds().intersects({ NGin::UI::Cursor::getPosition(), {1,1} })) {
		info_button.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("nebulogo.png"));
		info_screen.show();
	}
	else {
		info_button.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("info.png"));
		info_screen.hide();
	}
}

void MPlayer::Update(sf::RenderWindow& window)
{
	// forward skip
	if (next.activated() || seeker.getLevel() >= 0.9999f) {
		fileLoader.indexUp();
		if (!loadMusicFile()) window.close();
	}
	// backward skip
	else if (prev.activated()) {
		fileLoader.indexDown();
		if (!loadMusicFile()) window.close();
	}

	// start music
	if (play.getisActive() && BASS_ChannelIsActive(channel) != BASS_ACTIVE_PLAYING && channelLength != -1) {
		BASS_ChannelPlay(channel, FALSE);
		/*HANDLE POSSIBLE ERRORS*/
		if (BASS_ErrorGetCode() == BASS_ERROR_START) {
			
			NGin::Logger::log("ERROR! This porgram does not support "
				"changing playback device during runtime!", NGin::Logger::Severity::Error);
			
			system("pause");
			window.close();
		}
		NGin::Logger::log("Channel Started");
	}
	// stop music
	if (!play.getisActive() && BASS_ChannelIsActive(channel) == BASS_ACTIVE_PLAYING && channelLength != -1) {
		BASS_ChannelPause(channel);
		NGin::Logger::log("Channel Paused");
	}

	// volume adjust
	if (volume.getLevel() != volume_val / max_volume) {
		volume_val = volume.getLevel() * max_volume;
		BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, volume_val);
	}

	// sets the time string to curr time
	date_time.setString(NGin::Timer::getSysString());

	// the current channel length in percents
	float len_percent = float(BASS_ChannelGetPosition(channel, BASS_POS_BYTE)) / channelLength;

	// sets the seeker level to current length
	seeker.setLevel(len_percent);

	// if the level is changed via input
	if (seeker.getLevel() != len_percent) {
		const auto newLen = channelLength * seeker.getLevel();
		// apply changes to the channel
		BASS_ChannelSetPosition(channel, QWORD(newLen), BASS_POS_BYTE);
	}
}

void MPlayer::Compose(sf::RenderWindow& window)
{
	/*Image*/
	window.draw(banner);

	/*Control Panel*/
	window.draw(c_panel);
	window.draw(date_time);
	window.draw(volume);
	window.draw(vol_text);
	window.draw(inoutro);
	window.draw(inout_txt);

	/*Player*/
	window.draw(play);
	window.draw(seeker);
	window.draw(prev);
	window.draw(next);

	// only gets drawn if logo is hovered on
	window.draw(info_screen);

	// HAS TO BE LAST!
	window.draw(info_button);
}

bool MPlayer::loadMusicFile()
{
	if (fileLoader.hasChanged()) {
		BASS_ChannelPause(channel);
		BASS_SampleFree(sample);

		if (inoutro.getisActive()) {
			if (channelLength != 0) { // Outro
				channel = BASS_SampleGetChannel(outro, FALSE);
				BASS_ChannelPlay(channel, FALSE);

				while (BASS_ChannelIsActive(channel) == BASS_ACTIVE_PLAYING) {}
			}
			channel = BASS_SampleGetChannel(intro, FALSE);
			BASS_ChannelPlay(channel, FALSE);

			while (BASS_ChannelIsActive(channel) == BASS_ACTIVE_PLAYING) {}
		}

		std::string narrow_string(fileLoader.getCurrFileString());
		std::wstring wide_string = std::wstring(narrow_string.begin(), narrow_string.end());
		const wchar_t* location = wide_string.c_str();

		sample = BASS_SampleLoad(false, location, 0, 0, 1, BASS_DEVICE_STEREO);

		/*ERROR CHECKING*/
		if (BASS_ErrorGetCode() != BASS_OK) {
			while (BASS_ErrorGetCode() == BASS_ERROR_FILEOPEN) {
				NGin::Logger::log(fileLoader.getCurrFileString() + " FILE DOES NOT EXIST! ->"
					"\nPlease create it then press enter!", NGin::Logger::Severity::Warning);
				system("pause");
				sample = BASS_SampleLoad(false, location, 0, 0, 1, BASS_DEVICE_STEREO);
			}

			if (BASS_ErrorGetCode() != BASS_OK) {
				NGin::Logger::log("Unexpected error occured! Please contact Szoke Andras "
					"Lorand(the creator of the program)",
					NGin::Logger::Severity::Error);
				system("pause");
				return false;
			}
		}

		NGin::Logger::log(fileLoader.getCurrFileString() + " - Loaded");

		channel = BASS_SampleGetChannel(sample, FALSE);
		channelLength = BASS_ChannelGetLength(channel, BASS_POS_BYTE);

		// start playing by default
		BASS_ChannelPlay(channel, FALSE);
		play.setisActive(true);
		seeker.setLevel(0);
	}

	return true;
}
