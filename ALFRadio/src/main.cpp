#include "NGin/Base.h"
#include "NGin/UserInterface.h"

#include "bass.h" // mp3 player
#include "Application.h" // the functions of the application

BOOL CALLBACK RecordProc(HRECORD handle, const void* buffer, DWORD length, void* user)
{
	// copy microphone level into Application::microphoneLevel
	BASS_ChannelGetLevelEx(handle, &Application::microphoneLevel, 0.1f, BASS_LEVEL_MONO);
	return TRUE;
}

int main()
{
	// initialize music player
	BASS_Init(-1, 44100, 0, 0, NULL);

	// intialize console
	/*NGin::Logger::setConsoleFont({ 18, 18 }, true);
	NGin::Logger::setConsoleSize({ c_width, c_height });
	NGin::Logger::setConsoleName( con_name );*/

	// initialize render window
	sf::RenderWindow window{ sf::VideoMode(w_width, w_height), win_name };
	// the view that is going to be scaled
	sf::View view{ sf::FloatRect{0, 0, 1000, 600} };
	// apply the view to the window
	window.setView(view);
	// set framerate to 60 because there is no reason to have more
	window.setFramerateLimit(60);

	// load and set icon file
	sf::Image icon;
	if (!icon.loadFromFile("icon.png"))
		NGin::Logger::log("Icon \"icon.png\" is Missing!", NGin::Logger::Severity::Warning);
	else
		window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr()); // sets icon to sfml window

	// set default location of resources
	NGin::ResourceCodex::setLocation("assets/");

	// take first time measurement to not leave 0
	NGin::Timer::measureSysTime();

	// listen to microphone level
	int deviceNo = -1;
	BASS_DEVICEINFO dinfo;
	for (int a = 0; BASS_RecordGetDeviceInfo(a, &dinfo); a++)
		if ((dinfo.flags & BASS_DEVICE_ENABLED) && (dinfo.flags & BASS_DEVICE_TYPE_MASK) == BASS_DEVICE_TYPE_MICROPHONE) { // found an enabled microphone
			deviceNo = a;
			break;
		}
	if (deviceNo >= 0) {
		BASS_RecordInit(deviceNo); // initialize microphone recording device
		BASS_RecordStart(44100, 1, MAKELONG(0, 10), RecordProc, 0); // create a recording channel with 10ms period
	}

	// declare and set up application
	Application application;
	application.Setup();

	// signal reaching this point
	NGin::Logger::log("Aprily Radio - App Started");

	// APPLLICATION LOOP START

	while (window.isOpen())
	{
		NGin::Timer::measureSysTime();
		NGin::Timer::measureDeltaTime();

		sf::Event event;

		while (window.pollEvent(event))
		{
			// insert input handling in here

			if (event.type == sf::Event::Closed)
				window.close();

			// record the mouse into this static class
			NGin::UI::Cursor::followMouse(window);

			// handle app events
			application.handleEvents(event);
		}

		// clear render window
		window.clear();

		// update the logic
		application.Update(window);
		// compose the visuals
		application.Compose(window);

		// display new visuals
		window.display();
	}

	// APPLLICATION LOOP END

	return 0;
}
