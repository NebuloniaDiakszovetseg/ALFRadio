#include "Application.h"

void Application::Setup()
{
	cPanel.Setup();
	mPlayer.Setup();

	loadCurrInput();

	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, cPanel.getVolume());
	today = NGin::Timer::getSys().tm_mday;
}

void Application::handleEvents(const sf::Event& event)
{
	cPanel.handleEvents(event);
	mPlayer.handleEvents(event);
}

void Application::Update(sf::RenderWindow& window)
{
	// reloads files if changed
	if (Input::hasChanged()) {
		loadCurrInput();
	}

	/*CPanel - START*/
	// update control panel's visuals based on general app state
	cPanel.Update();
	
	// set volume when cPanel slider changes it
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, cPanel.getVolume());
	/*CPanel - END*/

	/*MPlayer - START*/
	// if date changes reset to first file and change date folder
	if (today != NGin::Timer::getSys().tm_mday) {
		Input::resetFile();
	}

	// update mPlayer's visuals based on channel's state
	mPlayer.Update(channel);

	// load next sample if current one ends
	// or exceeds time limit 
	if(curr_sample_length == BASS_ChannelGetLength(channel, BASS_POS_BYTE) &&
		mPlayer.getSeekerPos() >= mPlayer.getChannelLength() * 0.9999f ||
		Input::getEndTime(Input::getCurrIndex()) == NGin::Timer::getSysHMStr())
	{
		BASS_ChannelPause(channel);
		Input::nextFile();
		loadCurrInput();
	}

	// autoplay on Input-given times
	if (Input::getStartTime(Input::getCurrIndex()) == NGin::Timer::getSysHMStr())
	{
		mPlayer.setPlayActive(true);
	}

	// plays the music if it should play
	if (mPlayer.playMusic() && BASS_ChannelIsActive(channel) != BASS_ACTIVE_PLAYING) {
		BASS_ChannelPlay(channel, FALSE);
		if (BASS_ErrorGetCode() == BASS_ERROR_START) {
			NGin::Logger::log("Error Playing on main channel!", NGin::Logger::Severity::Error);
			system("pause");
			window.close();
		}
		else NGin::Logger::log("Channel Started -- Code: " + std::to_string(BASS_ErrorGetCode()));
	}
	// stops the music if it should stop
	if (mPlayer.stopMusic() && BASS_ChannelIsActive(channel) == BASS_ACTIVE_PLAYING) {
		BASS_ChannelPause(channel);
		NGin::Logger::log("Channel Paused -- Code: " + std::to_string(BASS_ErrorGetCode()));
	}

	// connect channel pos to seeker
	if(mPlayer.seekerMoved()){
		BASS_ChannelSetPosition(channel, mPlayer.getSeekerPos(), BASS_POS_BYTE);
	}
	/*
	if (curr_sample_length == BASS_ChannelGetLength(channel, BASS_POS_BYTE)) {
		cPanel.playIntro(channel);
	}

		BASS_ChannelStop(channel);
	channel = BASS_SampleGetChannel(sample, FALSE);*/
}

void Application::Compose(sf::RenderWindow& window)
{
	window.draw(cPanel);
	window.draw(mPlayer);
}

void Application::loadCurrInput()
{
	/*Avoid sound collision and free up unused memory*/
	BASS_ChannelPause(channel);
	BASS_SampleFree(sample);

	/*Convert location from string to wchar* */
	std::string narrow_string(Input::getCurrFileString());
	std::wstring wide_string = std::wstring(narrow_string.begin(), narrow_string.end());
	const wchar_t* location = wide_string.c_str();

	/*load sample with new location*/
	sample = BASS_SampleLoad(false, location, 0, 0, 1, BASS_DEVICE_STEREO);

	/*If input file cannot be opened, prompt user and wait for fix*/
	while (BASS_ErrorGetCode() == BASS_ERROR_FILEOPEN) {
		NGin::Logger::log(Input::getCurrFileString() + " FILE DOES NOT EXIST! ->"
			"\nPlease create it then press enter!", NGin::Logger::Severity::Warning);
		system("pause");
		sample = BASS_SampleLoad(false, location, 0, 0, 1, BASS_DEVICE_STEREO);
	}

	// signal to user that the file has been loaded successfully
	NGin::Logger::log(Input::getCurrFileString() + " - Loaded");

	Input::isLoaded();
	channel = BASS_SampleGetChannel(sample, FALSE);
	curr_sample_length = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
	
	// Reset Music Player
	mPlayer.setPlayActive(false);
	mPlayer.setSeekerPos(0);
}
