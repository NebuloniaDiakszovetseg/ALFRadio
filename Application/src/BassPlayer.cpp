#include "BassPlayer.h"
#include "Elements/Notification.h"

std::string BassPlayer::playingFilePath_ = "";
bool BassPlayer::playingFilePathChanged_ = false;
int BassPlayer::playingFileIndex_ = -1; // -1 not playing anything
bool BassPlayer::isPlaying_ = false;
bool BassPlayer::wasPlaying_ = true;
HSTREAM BassPlayer::mainChannel_;
bool BassPlayer::playingEnded_ = false;

bool BassPlayer::introLoaded_ = false;
bool BassPlayer::introOutroIsStreaming_ = false;
HSAMPLE BassPlayer::introSample_;
bool BassPlayer::outroLoaded_ = false;
HSAMPLE BassPlayer::outroSample_;
HCHANNEL BassPlayer::introOutroChannel_;
HRECORD BassPlayer::microphoneHandle_;
float BassPlayer::microphoneLevel_ = 0.0F;
int BassPlayer::microphoneDeviceNr_ = -1;
bool BassPlayer::microphoneWorks_ = false;

void BassPlayer::setPlayingFile(const int settingsIndex)
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

bool BassPlayer::playbackHasEnded()
{
	return playingEnded_;
}

std::string BassPlayer::getCurrentPositionString()
{
	if (introOutroIsStreaming_)
		return bytesToStringTime(BASS_ChannelGetPosition(introOutroChannel_, BASS_POS_BYTE));
	else
		return bytesToStringTime(BASS_ChannelGetPosition(mainChannel_, BASS_POS_BYTE));
}

std::string BassPlayer::getLengthString()
{
	if (introOutroIsStreaming_)
		return "~" + bytesToStringTime(BASS_ChannelGetLength(introOutroChannel_, BASS_POS_BYTE));
	else
		return "~" + bytesToStringTime(BASS_ChannelGetLength(mainChannel_, BASS_POS_BYTE));
}

void BassPlayer::setPercentagePlayed(const float percent)
{
	if (introOutroIsStreaming_)
	{
		auto length = BASS_ChannelGetLength(introOutroChannel_, BASS_POS_BYTE);

		QWORD position = static_cast<QWORD>(length * static_cast<double>(percent));
		BASS_ChannelSetPosition(introOutroChannel_, position, BASS_POS_BYTE);
	}
	else {
		auto length = BASS_ChannelGetLength(mainChannel_, BASS_POS_BYTE);

		QWORD position = static_cast<QWORD>(length * static_cast<double>(percent));
		BASS_ChannelSetPosition(mainChannel_, position, BASS_POS_BYTE);
	}
}

float BassPlayer::getPercentagePlayed()
{
	if (introOutroIsStreaming_)
	{
		// make sure the saved channel length is right
		double length = 
			static_cast<double>(BASS_ChannelGetLength(introOutroChannel_, BASS_POS_BYTE));
		double position = 
			static_cast<double>(BASS_ChannelGetPosition(introOutroChannel_, BASS_POS_BYTE));

		if (BASS_ChannelGetLength(introOutroChannel_, BASS_POS_BYTE) == -1 ||
			BASS_ChannelGetPosition(introOutroChannel_, BASS_POS_BYTE) == -1)
		{
			return 0.0F;
		}

		return static_cast<float>(position / length);
	}
	else
	{
		// make sure the saved channel length is right
		double length = 
			static_cast<double>(BASS_ChannelGetLength(mainChannel_, BASS_POS_BYTE));
		double position = 
			static_cast<double>(BASS_ChannelGetPosition(mainChannel_, BASS_POS_BYTE));

		if (BASS_ChannelGetLength(mainChannel_, BASS_POS_BYTE) == -1 ||
			BASS_ChannelGetPosition(mainChannel_, BASS_POS_BYTE) == -1)
		{
			return 0.0F;
		}

		return static_cast<float>(position / length);
	}
}

void BassPlayer::setup()
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

	introSample_ = 
		BASS_SampleLoad(false, Settings::getIntroLocation().c_str(), 0, 0, 1, BASS_DEVICE_MONO);
	introLoaded_ = (introSample_ != 0); // condition of loading going well

	outroSample_ =
		BASS_SampleLoad(false, Settings::getOutroLocation().c_str(), 0, 0, 1, BASS_DEVICE_MONO);
	outroLoaded_ = (outroSample_ != 0);

	// --- Initialize microphone START ------------------------------
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

	// --- Initialize microphone END ------------------------------
}

void BassPlayer::update()
{
	// TODO: TEST AND ENABLE!
	// handle changing device
	/*if (BASS_ErrorGetCode() == BASS_ERROR_DEVICE) {
		BASS_SetDevice(1);
		NG_LOG_NOTE("Output device has been changed!");
	}*/

	if(microphoneWorks_)
		measureMicLevel();

	// Load File
	if (playingFilePathChanged_)
	{
		playingFilePathChanged_ = false;

		BASS_ChannelStop(introOutroChannel_);
		introOutroIsStreaming_ = false;

		BASS_ChannelStop(mainChannel_);

		playingFilePath_ = Settings::getFilePath(playingFileIndex_);
		const void* file = playingFilePath_.c_str();
		mainChannel_ = BASS_StreamCreateFile(FALSE, file, 0, 0, 0);

		auto errorCode = BASS_ErrorGetCode();
		if (errorCode == BASS_ERROR_FILEOPEN) {
			NG_LOG_WARN("Cannot Play! - File not found!");
		}
		else if (errorCode == BASS_ERROR_FILEFORM) {
			NG_LOG_WARN("Cannot Play! - File in unrecognised format!");
			NG_LOG_WARN("Make sure files are in supported format! (mp3)");
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
