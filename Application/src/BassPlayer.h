#pragma once
#include "NGin.h"
#include "Settings.h"

#include "bass.h"

class BassPlayer {
public:
	// The BASS Library itself gets set up in class Application!!!
	static void setup();
	static void update();

	static void close() { BASS_Free(); }

	// gets index of file currently playing
	static int getPlayingFileIndex() { return playingFileIndex_; }

	// gets the index in settings of the file that has to be played
	static void setPlayingFileIndex(const int settingsIndex);

	static void setVolume(const float percent);
	static bool introIsLoaded() { return introLoaded_; }
	static bool outroIsLoaded() { return outroLoaded_; }
	static bool microphoneWorks() { return microphoneWorks_; }

	static void play() { wasPlaying_ = isPlaying_; isPlaying_ = true; }
	static void pause() { wasPlaying_ = isPlaying_; isPlaying_ = false; }

	static void prev();
	static void next();

	static float getMicrophoneLevel() { return microphoneLevel_; }

	// signals the end of playback to music player
	static bool playbackHasEnded() { return playingEnded_; }

	// returns current position of channel in mm:ss format
	static std::string getCurrentPositionString();
	// returns length of channel in mm:ss format
	static std::string getLengthString();

	static void setPercentagePlayed(const float percent);
	// returns how many percents of the song were played (in float 1-100% 0-0%)
	static float getPercentagePlayed();

	static bool isPlaying() { return isPlaying_; }

	static std::string bassVersionText();
private:
	static void initSpeakers();
	static void loadSamples();
	static void initMicrophone();

	static void loadPlayingFile();

	static DWORD activeOutputHandle() {
		if (introOutroIsStreaming_)
			return introOutroChannel_;
		else
			return mainChannel_;
	}

	static void streamIntro();
	static void streamOutro();

	// converts bytes into mm:ss string format
	static std::string bytesToStringTime(QWORD bytes);

	static std::string playingFilePath_;
	static bool playingFilePathChanged_;
	static int playingFileIndex_;

	static HSTREAM mainChannel_; // Handle for open stream
	static bool playingEnded_;
	static bool isPlaying_;
	static bool wasPlaying_;

	static bool introOutroIsStreaming_;
	static HCHANNEL introOutroChannel_;
	static HSAMPLE introSample_;  // piece getting played before break
	static bool introLoaded_;
	static HSAMPLE outroSample_; // piece gtting played after break
	static bool outroLoaded_;

	static HRECORD microphoneHandle_;
	static void measureMicLevel();
	static float microphoneLevel_;
	static int microphoneDeviceNr_;
	static bool microphoneWorks_;

	static constexpr int numberOfTries_ = 10;
};