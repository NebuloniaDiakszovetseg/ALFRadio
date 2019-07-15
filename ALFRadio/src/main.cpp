#include "NGin/Base.h"
#include "NGin/UserInterface.h"

#include "bass.h" // mp3 player
#include "Application.h" // the functions of the application

int main()
{
	// initialize music player
	BASS_Init(-1, 44100, 0, 0, NULL);

	// intialize console
	NGin::Logger::setConsoleFont({ 18, 18 }, true);
	NGin::Logger::setConsoleSize({ c_width, c_height });
	NGin::Logger::setConsoleName( con_name );

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

	// declare and set up application
	Application application;

	// run application on the render window
	application.run(window);

	return 0;
}
