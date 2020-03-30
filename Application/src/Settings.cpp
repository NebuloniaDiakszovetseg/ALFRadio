#include "Settings.h"
#include <fstream>
#include "Elements/Notification.h"

ng::json Settings::jsonFile_;
unsigned Settings::numberOfFiles_ = 0;

void Settings::load()
{
	std::ifstream in("settings.json");

	if (in.fail()) { // if failed to load
		in.close();
		// --- create a json file containing default settings ----
		// set keys to default settings
		jsonFile_[renderWindowKey][widthKey] = 1024;
		jsonFile_[renderWindowKey][heightKey] = 576;
		jsonFile_[renderWindowKey][typeKey] = static_cast<int>(ng::WINDOW_TYPE::WINDOW_UNRESIZEABLE);
		jsonFile_[consoleWindowKey][widthKey] = 800;
		jsonFile_[consoleWindowKey][heightKey] = 480;
		jsonFile_[consoleWindowKey][isDisabled] = false;
		jsonFile_[appVolumeKey] = 0.5F;
		jsonFile_[introLocKey] = std::string(INPUT_LOC) + std::string("intro.mp3");
		jsonFile_[outroLocKey] = std::string(INPUT_LOC) + std::string("outro.mp3");
		jsonFile_[autoInOutroKey] = false;
		jsonFile_[appVolumeKey] = 0.5F;
		jsonFile_[dimActiveKey] = false;
		jsonFile_[dimVolumeKey] = 0.2F;
		jsonFile_[dimThresholdKey] = 0.2F;
		jsonFile_[dimDurationKey] = 1.5F;

		// default duration of breaks
		numberOfFiles_ = 7;
		std::string starts[7] = { "7:25", "8:20", "9:20", "10:20", "11:20", "12:30", "13:30" };
		std::string ends[7] = { "7:31", "8:32", "9:32", "10:32", "11:42", "12:42", "13:42" };
		for (int i = 0; i < static_cast<int>(numberOfFiles_); i++) {
			jsonFile_[std::to_string(i)][startKey] = starts[i];
			jsonFile_[std::to_string(i)][endKey] = ends[i];
			jsonFile_[std::to_string(i)][fileKey] = std::to_string(i) + ".szunet.mp3";
		}

		// print out to file
		save();
	}
	else {
		in >> jsonFile_; // read json file
		in.close();

		// count number of mp3 files to be read
		int i = 0;
		std::string address = std::to_string(i);
		while (jsonFile_[address][startKey] != nullptr) {
			address = std::to_string(++i);
		}
		numberOfFiles_ = i;

		NG_LOG_INFO("Settings loaded successfully!");
	}
}

void Settings::save()
{
	std::ofstream out("settings.json");

	if (out.fail())
	{
		Notification::popup("Error!", 
			"Unexpected error while writing settings.json file!\n"
			"Please contact Szoke Andras-Lorand if error persists!");

		out.close();
		ng::Main::closeWindow();
	}
	else {
		jsonFile_["!comment1"] = "You can add or remove breaks here and change system settings manually!";
		jsonFile_["!comment2"] = "However proceed with caution! Things might break roun' here.";
		jsonFile_["!comment3"] = "If things do break delete this file and start program to reset defaults";
		jsonFile_["!comment4"] = "For more information contact me, Sz.Lori on social media :D";;

		out << jsonFile_.dump(4); // print out in an organised way
		out.close();

		NG_LOG_INFO("Settings created/saved successfully!");
	}
}

