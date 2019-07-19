#include "Application.h"

void Application::Setup()
{
	cPanel.Setup();
	mPlayer.Setup();

	loadCurrInput();

	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, cPanel.getVolume());
	today = NGin::Timer::getSys().tm_mday; // save today as current date
}

void Application::handleEvents(const sf::Event& event)
{
	cPanel.handleEvents(event);
	mPlayer.handleEvents(event);
}

void Application::Update(sf::RenderWindow& window)
{
	// if date changes reset to first file and change date folder
	if (today != NGin::Timer::getSys().tm_mday) {
		today = NGin::Timer::getSys().tm_mday; // set new date
		Input::resetFile();
	}
	// reloads files if changed
	if (Input::hasChanged()) {
		loadCurrInput();
	}

	// update control panel's visuals based on general app state
	cPanel.Update();

	// update mPlayer's visuals based on channel's state
	mPlayer.Update(channel);
	
	// set volume when cPanel slider changes it
	BASS_ChannelSetAttribute(channel, BASS_ATTRIB_VOL, cPanel.getVolume());

	// load next sample if current one ends
	// or exceeds time limit 
	if(curr_sample_length == BASS_ChannelGetLength(channel, BASS_POS_BYTE) &&
		mPlayer.getSeekerPos() >= mPlayer.getChannelLength() * 0.9999f ||
		Input::getEndTime(Input::getCurrIndex()) == NGin::Timer::getSysHMStr())
	{
		// prompt the user about the forceful stop
		if (Input::getEndTime(Input::getCurrIndex()) == NGin::Timer::getSysHMStr())
		{
			NGin::Logger::log("Time limit exceeded -> Channel STOPPED automatically", NGin::Logger::Severity::Warning);
		}

		// Auto Outro
		if (cPanel.inoutActive()) {
			cPanel.playOutro(channel);
			// Continue Playing
			mPlayer.setPlayActive(true);
		}
		else {
			// Load next file
			BASS_ChannelStop(channel);
			Input::nextFile();
			loadCurrInput();
		}
	}

	// If Auto-Outro Stopped
	if (cPanel.outroStopped(channel))
	{
		// Load next file
		BASS_ChannelStop(channel);
		Input::nextFile();
		loadCurrInput();
	}
	// If Auto-Intro Stopped
	if (cPanel.introStopped(channel))
	{
		// Start the sample
		BASS_ChannelStop(channel);
		channel = BASS_SampleGetChannel(sample, FALSE);

		// Continue Playing
		mPlayer.setPlayActive(true);
		mPlayer.setSeekerPos(0.0002f); // bypass infinite intro loop bug
		BASS_ChannelPlay(channel, FALSE);
	}

	// Autoplay on Input-given times
	if (Input::getStartTime(Input::getCurrIndex()) == NGin::Timer::getSysHMStr())
	{
		mPlayer.setPlayActive(true);
	}

	// starts playing the music if it should play
	if (mPlayer.playMusic() && BASS_ChannelIsActive(channel) != BASS_ACTIVE_PLAYING) {

		// Auto Intro
		if (cPanel.inoutActive() && mPlayer.getSeekerPos() <= mPlayer.getChannelLength() * 0.0001f &&
			curr_sample_length == BASS_ChannelGetLength(channel, BASS_POS_BYTE))
		{
			cPanel.playIntro(channel);
		}

		BASS_ChannelPlay(channel, FALSE);
		if (BASS_ErrorGetCode() == BASS_ERROR_START)
		{
			NGin::Logger::log("Error Playing on main channel! -> Please restart the program \n"
				"->If the error persists, contact me (Szoke Andras-Lorand) at \nhttp://www.nebulonia.ro/creators",
				NGin::Logger::Severity::Error);
			
			system("pause");
			window.close();
		}
		else NGin::Logger::log("Channel Started -- Code: " + std::to_string(BASS_ErrorGetCode()));
	}
	// pauses the music if manually paused
	if (mPlayer.stopMusic() && BASS_ChannelIsActive(channel) == BASS_ACTIVE_PLAYING) {
		BASS_ChannelPause(channel);
		NGin::Logger::log("Channel Paused -- Code: " + std::to_string(BASS_ErrorGetCode()));
	}

	// connect channel pos to seeker
	if(mPlayer.seekerMoved()){
		BASS_ChannelSetPosition(channel, mPlayer.getSeekerPos(), BASS_POS_BYTE);
	}
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
