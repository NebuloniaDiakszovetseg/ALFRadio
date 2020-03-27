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
	static int getPlayingFileIndex() { return playingFileIndex; }

	// gets the index in settings of the file that has to be played
	static void setPlayingFile(const int settingsIndex);
	static void setVolume(const float percent);
	static bool introIsLoaded() { return introLoaded; }
	static bool outroIsLoaded() { return outroLoaded; }
	static bool getCanDim() { return microphoneWorks; }

	static void play() { wasPlaying = isPlaying; isPlaying = true; }
	static void pause() { wasPlaying = isPlaying; isPlaying = false; }

	static void prev();
	static void next();

	static float getMicrophoneLevel() { return microphoneLevel; }

	// signals the end of playback to music player
	static bool playbackHasEnded();

	// returns current position of channel in mm:ss format
	static std::string getCurrentPositionString();
	// returns length of channel in mm:ss format
	static std::string getLengthString();

	static void setPercentagePlayed(const float percent);
	// returns how many percents of the song were played (in float 1-100% 0-0%)
	static float getPercentagePlayed();

	static bool getIsPlaying() { return isPlaying; }

	static std::string bassVersionText();
private:
	static void streamIntro();
	static void streamOutro();

	// converts bytes into mm:ss string format
	static std::string bytesToStringTime(QWORD bytes);

	static int playingFileIndex;
	static bool isPlaying;
	static bool wasPlaying;

	static std::string playingFilePath;
	static bool playingFilePathChanged;

	static HSTREAM mainChannel; // Handle for open stream
	static bool playingEnded;

	static bool introOutroIsStreaming;
	static HCHANNEL introOutroChannel;
	static HSAMPLE introSample;  // piece getting played before break
	static bool introLoaded;
	static HSAMPLE outroSample; // piece gtting played after break
	static bool outroLoaded;

	static HRECORD microphoneHandle;
	static void measureMicLevel();
	static float microphoneLevel;
	static int microphoneDeviceNr;
	static bool microphoneWorks;
};