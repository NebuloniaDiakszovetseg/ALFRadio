#include "BassPlayer.h"

std::string BassPlayer::playingFilePath = "";
bool BassPlayer::playingFilePathChanged = false;
int BassPlayer::playingFileIndex = -1; // -1 not playing anything
bool BassPlayer::isPlaying = false;
bool BassPlayer::wasPlaying = true;
HSTREAM BassPlayer::mainChannel;
bool BassPlayer::playingEnded = false;

bool BassPlayer::introLoaded = false;
bool BassPlayer::introOutroIsStreaming = false;
HSAMPLE BassPlayer::introSample;
bool BassPlayer::outroLoaded = false;
HSAMPLE BassPlayer::outroSample;
HCHANNEL BassPlayer::introOutroChannel;
HRECORD BassPlayer::microphoneHandle;
float BassPlayer::microphoneLevel = 0.0F;
int BassPlayer::microphoneDeviceNr = -1;
bool BassPlayer::microphoneWorks = false;

void BassPlayer::setPlayingFile(const int settingsIndex)
{
	if (settingsIndex > static_cast<int>(Settings::getNumOfFiles()) || settingsIndex < 0) {
		NG_LOG_WARN("BassPlayer -> settingsIndex(", settingsIndex, ") out of bounds!");
		NG_LOG_WARN("Playing file not set! Unexpected behaviour might occur!");
	}
	else {
		playingFileIndex = settingsIndex;
		playingFilePathChanged = true;
	}
}

void BassPlayer::prev()
{
	if (playingFileIndex > 0) {
		playingFileIndex--;
		playingFilePathChanged = true;
	}
}

void BassPlayer::next()
{
	if (playingFileIndex + 1 < static_cast<int>(Settings::getNumOfFiles()))
	{
		playingFileIndex++;
		playingFilePathChanged = true;
	}
}

bool BassPlayer::playbackHasEnded()
{
	return playingEnded;
}

std::string BassPlayer::getCurrentPositionString()
{
	if (introOutroIsStreaming)
		return bytesToStringTime(BASS_ChannelGetPosition(introOutroChannel, BASS_POS_BYTE));
	else
		return bytesToStringTime(BASS_ChannelGetPosition(mainChannel, BASS_POS_BYTE));
}

std::string BassPlayer::getLengthString()
{
	if (introOutroIsStreaming)
		return "~" + bytesToStringTime(BASS_ChannelGetLength(introOutroChannel, BASS_POS_BYTE));
	else
		return "~" + bytesToStringTime(BASS_ChannelGetLength(mainChannel, BASS_POS_BYTE));
}

void BassPlayer::setPercentagePlayed(const float percent)
{
	if (introOutroIsStreaming)
	{
		auto length = BASS_ChannelGetLength(introOutroChannel, BASS_POS_BYTE);

		QWORD position = static_cast<QWORD>(length * static_cast<double>(percent));
		BASS_ChannelSetPosition(introOutroChannel, position, BASS_POS_BYTE);
	}
	else {
		auto length = BASS_ChannelGetLength(mainChannel, BASS_POS_BYTE);

		QWORD position = static_cast<QWORD>(length * static_cast<double>(percent));
		BASS_ChannelSetPosition(mainChannel, position, BASS_POS_BYTE);
	}
}

float BassPlayer::getPercentagePlayed()
{
	if (introOutroIsStreaming)
	{
		// make sure the saved channel length is right
		double length = 
			static_cast<double>(BASS_ChannelGetLength(introOutroChannel, BASS_POS_BYTE));
		double position = 
			static_cast<double>(BASS_ChannelGetPosition(introOutroChannel, BASS_POS_BYTE));

		if (BASS_ChannelGetLength(introOutroChannel, BASS_POS_BYTE) == -1 ||
			BASS_ChannelGetPosition(introOutroChannel, BASS_POS_BYTE) == -1)
		{
			return 0.0F;
		}

		return static_cast<float>(position / length);
	}
	else
	{
		// make sure the saved channel length is right
		double length = 
			static_cast<double>(BASS_ChannelGetLength(mainChannel, BASS_POS_BYTE));
		double position = 
			static_cast<double>(BASS_ChannelGetPosition(mainChannel, BASS_POS_BYTE));

		if (BASS_ChannelGetLength(mainChannel, BASS_POS_BYTE) == -1 ||
			BASS_ChannelGetPosition(mainChannel, BASS_POS_BYTE) == -1)
		{
			return 0.0F;
		}

		return static_cast<float>(position / length);
	}
}

void BassPlayer::setup()
{
	BASS_Init(-1, 44100, BASS_DEVICE_MONO, ng::Main::getWindowHandle(), NULL);

	introSample = 
		BASS_SampleLoad(false, Settings::getIntroLocation().c_str(), 0, 0, 1, BASS_DEVICE_MONO);
	introLoaded = (introSample != 0); // condition of loading going well

	outroSample =
		BASS_SampleLoad(false, Settings::getOutroLocation().c_str(), 0, 0, 1, BASS_DEVICE_MONO);
	outroLoaded = (outroSample != 0);

	// --- Initialize microphone START ------------------------------
	microphoneDeviceNr = -1;
	BASS_DEVICEINFO dinfo;
	// find an enabled microphone
	for (int a = 0; BASS_RecordGetDeviceInfo(a, &dinfo); a++) {
		if ((dinfo.flags & BASS_DEVICE_ENABLED) &&
			(dinfo.flags & BASS_DEVICE_TYPE_MASK) == BASS_DEVICE_TYPE_MICROPHONE)
		{
			microphoneDeviceNr = a;
			break;
		}
	}

	microphoneWorks = true;
	if (microphoneDeviceNr >= 0) {
		// initialize microphone recording device
		BASS_RecordInit(microphoneDeviceNr);
		// create a recording channel with 10ms period
		microphoneHandle = BASS_RecordStart(44100, 1, MAKELONG(0, 10), NULL, 0);
	
		if (BASS_ErrorGetCode() != BASS_OK)
		{
			microphoneWorks = false;
			NG_LOG_WARN("Error initializing microphone!");
			NG_LOG_WARN("Dim if microphone active function disabled!");
		}
	}
	else {
		microphoneWorks = false;
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

	if(microphoneWorks)
		measureMicLevel();

	// Load File
	if (playingFilePathChanged)
	{
		playingFilePathChanged = false;

		BASS_ChannelStop(introOutroChannel);
		introOutroIsStreaming = false;

		BASS_ChannelStop(mainChannel);

		playingFilePath = Settings::getFilePath(playingFileIndex);
		const void* file = playingFilePath.c_str();
		mainChannel = BASS_StreamCreateFile(FALSE, file, 0, 0, 0);

		auto errorCode = BASS_ErrorGetCode();
		if (errorCode == BASS_ERROR_FILEOPEN) {
			NG_LOG_WARN("Cannot Play! - File not found!");
		}
		else if (errorCode == BASS_ERROR_FILEFORM) {
			NG_LOG_WARN("Cannot Play! - File in unrecognised format!");
			NG_LOG_WARN("Make sure files are in supported format! (mp3)");
		}

		// if it was playing keep playing = send a "fake" signal into play logic
		if (isPlaying) {
			wasPlaying = false;
		}
	}

	// stop or play outro if the end of file has been reached
	if (BASS_ChannelGetPosition(mainChannel, BASS_POS_BYTE) >=
		BASS_ChannelGetLength(mainChannel, BASS_POS_BYTE) &&
		BASS_ChannelGetLength(mainChannel, BASS_POS_BYTE) != -1)
	{
		BASS_ChannelPause(mainChannel);
		BASS_ChannelSetPosition(mainChannel, 0, BASS_POS_BYTE);

		// start playing outro
		if (Settings::getAutoIntroOutro()) {
			streamOutro();
		}
		else {
			playingEnded = true;
			pause();
		}
	}
	else {
		playingEnded = false;
	}

	// make sure disabling intro/outro on the fly resets everything properly
	if (!Settings::getAutoIntroOutro() && introOutroIsStreaming) {
		introOutroIsStreaming = false;
		BASS_ChannelStop(introOutroChannel);

		// intro && if it was playing
		if (isPlaying) {
			if (BASS_ChannelGetPosition(mainChannel, BASS_POS_BYTE) == 2)
			{
				// keep playing = send a "fake" signal into play logic
				wasPlaying = false;
			}
			else if (BASS_ChannelGetPosition(mainChannel, BASS_POS_BYTE) == 0)
			{
				// stop playing
				playingEnded = true;
				pause();
			}
		}
	}

	// resume playing main channel after intro/outro finished
	if (isPlaying && introOutroIsStreaming &&
		BASS_ChannelIsActive(introOutroChannel) != BASS_ACTIVE_PLAYING &&
		BASS_ChannelGetPosition(introOutroChannel, BASS_POS_BYTE) == 0)
	{
		introOutroIsStreaming = false;
		BASS_ChannelStop(introOutroChannel);

		// resume after intro
		if (BASS_ChannelGetPosition(mainChannel, BASS_POS_BYTE) == 2)
		{
			BASS_ChannelPlay(mainChannel, FALSE);
			play();
		}
		// resume after outro
		else if (BASS_ChannelGetPosition(mainChannel, BASS_POS_BYTE) == 0)
		{
			playingEnded = true;
			pause();
		}
	}

	// Play Toggle
	if (isPlaying && !wasPlaying)
	{
		// intro should resume
		if (introOutroIsStreaming) {
			BASS_ChannelPlay(introOutroChannel, FALSE);
		}
		// if intro should play because playing has just been started
		else if (Settings::getAutoIntroOutro() &&
			BASS_ChannelGetPosition(mainChannel, BASS_POS_BYTE) == 0)
		{
			BASS_ChannelPause(mainChannel);
			// miss first two bytes to signal playing the intro
			BASS_ChannelSetPosition(mainChannel, 2, BASS_POS_BYTE);

			streamIntro();
		}
		else {
			introOutroIsStreaming = false;
			BASS_ChannelPlay(mainChannel, FALSE);
		}
	}
	// Pause Toggle
	else if (!isPlaying && wasPlaying)
	{
		if (introOutroIsStreaming)
			BASS_ChannelPause(introOutroChannel);
		
		BASS_ChannelPause(mainChannel);
	}

}

void BassPlayer::setVolume(const float percent)
{
	BASS_ChannelSetAttribute(mainChannel, BASS_ATTRIB_VOL, percent);
	BASS_ChannelSetAttribute(introOutroChannel, BASS_ATTRIB_VOL, percent);
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
	if (BASS_ChannelIsActive(introOutroChannel) != BASS_ACTIVE_PLAYING)
	{
		introOutroChannel = BASS_SampleGetChannel(introSample, FALSE);
		BASS_ChannelPlay(introOutroChannel, FALSE);
		introOutroIsStreaming = true;
	}
}

void BassPlayer::streamOutro()
{
	if (BASS_ChannelIsActive(introOutroChannel) != BASS_ACTIVE_PLAYING)
	{
		introOutroChannel = BASS_SampleGetChannel(outroSample, FALSE);
		BASS_ChannelPlay(introOutroChannel, FALSE);
		introOutroIsStreaming = true;
	}
}

std::string BassPlayer::bytesToStringTime(QWORD bytes)
{
	if (bytes != -1) { // if successfull
		double rawLength = BASS_ChannelBytes2Seconds(mainChannel, bytes);
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

	DWORD level = BASS_ChannelGetLevel(microphoneHandle);
	// get left channel only (it is mono so it doesn't matter)
	level = LOWORD(level);

	// error
	if (level == -1) {
		NG_LOG_WARN("Cannot record microphone! Recording disabled!");
		NG_LOG_WARN("BASS ERROR CODE: ", BASS_ErrorGetCode());
		microphoneWorks = false;
	}
	else {
		if (level == 0) microphoneLevel = 0;
		else {
			BassPlayer::microphoneLevel =
				static_cast<float>(level) / static_cast<float>(maxLevel);
		}
	}
}
