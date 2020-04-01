#include "BassPlayer.h"
#include "Elements/Notification.h"
#include "RandomList.h"

std::string BassPlayer::playingFilePath_ = "";
bool BassPlayer::playingFilePathChanged_ = false;
int BassPlayer::playingFileIndex_ = -1; // -1 not playing anything

HSTREAM BassPlayer::mainChannel_;
bool BassPlayer::playingEnded_ = false;
bool BassPlayer::isPlaying_ = false;
bool BassPlayer::wasPlaying_ = true;

bool BassPlayer::introOutroIsStreaming_ = false;
HCHANNEL BassPlayer::introOutroChannel_;
HSAMPLE BassPlayer::introSample_;
bool BassPlayer::introLoaded_ = false;
HSAMPLE BassPlayer::outroSample_;
bool BassPlayer::outroLoaded_ = false;

HRECORD BassPlayer::microphoneHandle_;
float BassPlayer::microphoneLevel_ = 0.0F;
int BassPlayer::microphoneDeviceNr_ = -1;
bool BassPlayer::microphoneWorks_ = false;

void BassPlayer::setPlayingFileIndex(const int settingsIndex)
{
	if (settingsIndex > static_cast<int>(Settings::getNumOfFiles()) || settingsIndex < 0) {
		NG_LOG_WARN("BassPlayer -> settingsIndex(", settingsIndex, ") out of bounds!");
		NG_LOG_WARN("Playing file not set! Unexpected behaviour might occur!");
	}
	else {
		playingFileIndex_ = settingsIndex;
		playingFilePathChanged_ = true;
	}
}

void BassPlayer::prev()
{
	if (playingFileIndex_ > 0) {
		playingFileIndex_--;
		playingFilePathChanged_ = true;
	}
}

void BassPlayer::next()
{
	if (playingFileIndex_ + 1 < static_cast<int>(Settings::getNumOfFiles()))
	{
		playingFileIndex_++;
		playingFilePathChanged_ = true;
	}
}

std::string BassPlayer::getCurrentPositionString()
{
	return
		bytesToStringTime(BASS_ChannelGetPosition(activeOutputHandle(), BASS_POS_BYTE));
}

std::string BassPlayer::getLengthString()
{
	return "~" + 
		bytesToStringTime(BASS_ChannelGetLength(activeOutputHandle(), BASS_POS_BYTE));
}

void BassPlayer::setPercentagePlayed(const float percent)
{
	auto length = BASS_ChannelGetLength(activeOutputHandle(), BASS_POS_BYTE);

	QWORD position = static_cast<QWORD>(length * static_cast<double>(percent));
	BASS_ChannelSetPosition(activeOutputHandle(), position, BASS_POS_BYTE);
}

float BassPlayer::getPercentagePlayed()
{
	// make sure the saved channel length is right
	double length = 
		static_cast<double>(BASS_ChannelGetLength(activeOutputHandle(), BASS_POS_BYTE));
	double position = 
		static_cast<double>(BASS_ChannelGetPosition(activeOutputHandle(), BASS_POS_BYTE));

	if (BASS_ChannelGetLength(activeOutputHandle(), BASS_POS_BYTE) == -1 ||
		BASS_ChannelGetPosition(activeOutputHandle(), BASS_POS_BYTE) == -1)
	{
		return 0.0F;
	}

	return static_cast<float>(position / length);
}

void BassPlayer::setup()
{
	// Include a "Default" entry in the output device list
	// aka stick to deafult output device
	BASS_SetConfig(
		BASS_CONFIG_DEV_DEFAULT,
		TRUE
	);

	initSpeakers();
	loadSamples();
	initMicrophone();
}

void BassPlayer::update()
{
	if(microphoneWorks_)
		measureMicLevel();

	// Load File
	if (playingFilePathChanged_)
	{
		playingFilePathChanged_ = false;

		BASS_ChannelStop(introOutroChannel_);
		introOutroIsStreaming_ = false;

		BASS_ChannelStop(mainChannel_);

		loadPlayingFile();

		auto errorCode = BASS_ErrorGetCode();
		if (errorCode == BASS_ERROR_FILEOPEN) // if failed to open file
		{
			NG_LOG_WARN("Cannot Play! - File not found!");
		}
		else if (errorCode == BASS_ERROR_FILEFORM) {
			NG_LOG_WARN("Cannot Play! - File in unrecognised format!");
			NG_LOG_WARN("Make sure files are in supported format! (mp3)");
		}

		// randomize if files not found
		if (errorCode != BASS_OK && Settings::getFNFIsEnabled())
		{
			NG_LOG_INFO("Random directory will be choosen"
				" out of ", RANDOM_LIST_FILE, " entries");

			std::string randFilePath;
			int tries = 0; // count number of times tried
			do {
				tries++;

				RandomList::generateRandomPath();

				// set and load file
				randFilePath = RandomList::getFilePath(playingFileIndex_);
				const void* file = randFilePath.c_str();
				mainChannel_ = BASS_StreamCreateFile(FALSE, file, 0, 0, 0);

				errorCode = BASS_ErrorGetCode(); // get current error code
			} while (tries <= numberOfTries_ && errorCode != BASS_OK);
			// do until you succeed or fail numberOfTries_ times

			// if failed prompt user with an error message
			if (tries == numberOfTries_) {
				NG_LOG_WARN("Could not find a random file"
					" with fitting description!");
			}
			else {
				playingFilePath_ = randFilePath;
				RandomList::markAsUsed();
				NG_LOG_INFO("Path choosen: ", RandomList::getFolderPath());
			}
		}

		// if it was playing keep playing = send a "fake" signal into play logic
		if (isPlaying_) {
			wasPlaying_ = false;
		}
	}

	// stop or play outro if the end of file has been reached
	if (BASS_ChannelGetPosition(mainChannel_, BASS_POS_BYTE) >=
		BASS_ChannelGetLength(mainChannel_, BASS_POS_BYTE) &&
		BASS_ChannelGetLength(mainChannel_, BASS_POS_BYTE) != -1)
	{
		BASS_ChannelPause(mainChannel_);
		BASS_ChannelSetPosition(mainChannel_, 0, BASS_POS_BYTE);

		// start playing outro
		if (Settings::getAutoIntroOutro()) {
			streamOutro();
		}
		else {
			playingEnded_ = true;
			pause();
		}
	}
	else {
		playingEnded_ = false;
	}

	// make sure disabling intro/outro on the fly resets everything properly
	if (!Settings::getAutoIntroOutro() && introOutroIsStreaming_) {
		introOutroIsStreaming_ = false;
		BASS_ChannelStop(introOutroChannel_);

		// intro && if it was playing
		if (isPlaying_) {
			if (BASS_ChannelGetPosition(mainChannel_, BASS_POS_BYTE) == 2)
			{
				// keep playing = send a "fake" signal into play logic
				wasPlaying_ = false;
			}
			else if (BASS_ChannelGetPosition(mainChannel_, BASS_POS_BYTE) == 0)
			{
				// stop playing
				playingEnded_ = true;
				pause();
			}
		}
	}

	// resume playing main channel after intro/outro finished
	if (isPlaying_ && introOutroIsStreaming_ &&
		BASS_ChannelIsActive(introOutroChannel_) != BASS_ACTIVE_PLAYING &&
		BASS_ChannelGetPosition(introOutroChannel_, BASS_POS_BYTE) == 0)
	{
		introOutroIsStreaming_ = false;
		BASS_ChannelStop(introOutroChannel_);

		// resume after intro
		if (BASS_ChannelGetPosition(mainChannel_, BASS_POS_BYTE) == 2)
		{
			BASS_ChannelPlay(mainChannel_, FALSE);
			play();
		}
		// resume after outro
		else if (BASS_ChannelGetPosition(mainChannel_, BASS_POS_BYTE) == 0)
		{
			playingEnded_ = true;
			pause();
		}
	}

	// Play Toggle
	if (isPlaying_ && !wasPlaying_)
	{
		// intro should resume
		if (introOutroIsStreaming_) {
			BASS_ChannelPlay(introOutroChannel_, FALSE);
		}
		// if intro should play because playing has just been started
		else if (Settings::getAutoIntroOutro() &&
			BASS_ChannelGetPosition(mainChannel_, BASS_POS_BYTE) == 0)
		{
			BASS_ChannelPause(mainChannel_);
			// miss first two bytes to signal playing the intro
			BASS_ChannelSetPosition(mainChannel_, 2, BASS_POS_BYTE);

			streamIntro();
		}
		else {
			introOutroIsStreaming_ = false;
			BASS_ChannelPlay(mainChannel_, FALSE);
		}
	}
	// Pause Toggle
	else if (!isPlaying_ && wasPlaying_)
	{
		if (introOutroIsStreaming_)
			BASS_ChannelPause(introOutroChannel_);
		
		BASS_ChannelPause(mainChannel_);
	}
}

void BassPlayer::setVolume(const float percent)
{
	BASS_ChannelSetAttribute(mainChannel_, BASS_ATTRIB_VOL, percent);
	BASS_ChannelSetAttribute(introOutroChannel_, BASS_ATTRIB_VOL, percent);
}

std::string BassPlayer::bassVersionText()
{
	int version = BASS_GetVersion();

	// holds conversion of version encoded in hex
	std::string text = "";

	// expected 8 characters grouped by 2 from 4byte hexadecimal
	for (int j = 1; j <= 4; j++)
	{
		// conversion
		std::string convertor = std::to_string(version % 256);
		convertor.append(text);
		text = convertor;

		version /= 256; // because 2-grouped hexadecimal

		if (j != 4) // don't put first point ( ->.******* )
			text.insert(text.begin(), '.');
	}

	return text;
}

void BassPlayer::initSpeakers()
{
	BASS_Init(-1, 44100, BASS_DEVICE_MONO, ng::Main::getWindowHandle(), NULL);
	if (BASS_ErrorGetCode() != BASS_OK)
	{
		NG_LOG_ERROR("Error! Failed to initialize sound output!");
		NG_LOG_ERROR("BASS ERROR CODE: ", BASS_ErrorGetCode());
		NG_LOG_NOTE("Go to audio devices in start menu and check if output device"
			" is enabled");

		Notification::popup("Error!",
			"                  Sound output failed to initialize!\n"
			"Please check if there is a proper audio output device!");

		ng::Main::closeWindow();
	}
}

void BassPlayer::loadSamples()
{
	BASS_SampleFree(introSample_);
	BASS_SampleFree(outroSample_);

	introSample_ =
		BASS_SampleLoad(false, Settings::getIntroLocation().c_str(), 0, 0, 1, BASS_DEVICE_MONO);
	introLoaded_ = (introSample_ != 0); // condition of loading going well

	outroSample_ =
		BASS_SampleLoad(false, Settings::getOutroLocation().c_str(), 0, 0, 1, BASS_DEVICE_MONO);
	outroLoaded_ = (outroSample_ != 0);
}

void BassPlayer::initMicrophone()
{
	microphoneDeviceNr_ = -1;
	BASS_DEVICEINFO dinfo;
	// find an enabled microphone
	for (int a = 0; BASS_RecordGetDeviceInfo(a, &dinfo); a++) {
		if ((dinfo.flags & BASS_DEVICE_ENABLED) &&
			(dinfo.flags & BASS_DEVICE_TYPE_MASK) == BASS_DEVICE_TYPE_MICROPHONE)
		{
			microphoneDeviceNr_ = a;
			break;
		}
	}

	microphoneWorks_ = true;
	if (microphoneDeviceNr_ >= 0) {
		// initialize microphone recording device
		BASS_RecordInit(microphoneDeviceNr_);
		// create a recording channel with 10ms period
		microphoneHandle_ = BASS_RecordStart(44100, 1, MAKELONG(0, 10), NULL, 0);

		if (BASS_ErrorGetCode() != BASS_OK)
		{
			microphoneWorks_ = false;
			NG_LOG_WARN("Error initializing microphone!");
			NG_LOG_WARN("Dim if microphone active function disabled!");
		}
	}
	else {
		microphoneWorks_ = false;
		NG_LOG_WARN("Cannot initialize microphone! No enabled device found!");
		NG_LOG_WARN("Dim if microphone active function disabled!");
	}

}

void BassPlayer::loadPlayingFile()
{
	if (playingFileIndex_ != -1)
	{
		BASS_StreamFree(mainChannel_);
		playingFilePath_ = Settings::getFilePath(playingFileIndex_);
		const void* file = playingFilePath_.c_str();
		mainChannel_ = BASS_StreamCreateFile(FALSE, file, 0, 0, 0);
	}
	else
		NG_LOG_WARN("Playing file not set but loaded! Unexpected behaviour might occur.");
}

void BassPlayer::streamIntro()
{
	if (BASS_ChannelIsActive(introOutroChannel_) != BASS_ACTIVE_PLAYING)
	{
		introOutroChannel_ = BASS_SampleGetChannel(introSample_, FALSE);
		BASS_ChannelPlay(introOutroChannel_, FALSE);
		introOutroIsStreaming_ = true;
	}
}

void BassPlayer::streamOutro()
{
	if (BASS_ChannelIsActive(introOutroChannel_) != BASS_ACTIVE_PLAYING)
	{
		introOutroChannel_ = BASS_SampleGetChannel(outroSample_, FALSE);
		BASS_ChannelPlay(introOutroChannel_, FALSE);
		introOutroIsStreaming_ = true;
	}
}

std::string BassPlayer::bytesToStringTime(QWORD bytes)
{
	if (bytes != -1) { // if successfull
		double rawLength = BASS_ChannelBytes2Seconds(mainChannel_, bytes);
		long long lengthInMin = static_cast<int>(rawLength / 60);
		long long lengthInSec = static_cast<int>(rawLength - (lengthInMin * 60));

		std::string lengthStr = "";

		if (lengthInMin < 10) lengthStr = "0" + std::to_string(lengthInMin) + ":";
		else lengthStr = std::to_string(lengthInMin) + ":";

		if (lengthInSec < 10) lengthStr += "0" + std::to_string(lengthInSec);
		else lengthStr += std::to_string(lengthInSec);

		return lengthStr;
	}
	else return "00:00";
}

void BassPlayer::measureMicLevel()
{
	const DWORD maxLevel = 32768;

	DWORD level = BASS_ChannelGetLevel(microphoneHandle_);
	// get left channel only (it is mono so it doesn't matter)
	level = LOWORD(level);

	// error
	if (level == -1) {
		NG_LOG_WARN("Cannot record microphone! Recording disabled!");
		NG_LOG_WARN("BASS ERROR CODE: ", BASS_ErrorGetCode());
		microphoneWorks_ = false;
	}
	else {
		if (level == 0) microphoneLevel_ = 0;
		else {
			BassPlayer::microphoneLevel_ =
				static_cast<float>(level) / static_cast<float>(maxLevel);
		}
	}
}
