#include "Application.h"

void Application::run(sf::RenderWindow& window)
{
	NGin::Timer::measureSysTime();

	// signal reaching this point
	NGin::Logger::log("Aprily Radio - App Started");

	mPlayer.Setup(window);

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
			mPlayer.handleEvents(event);
		}

		// clear render window
		window.clear();

		// update the logic
		mPlayer.Update(window);
		// compose the visuals
		mPlayer.Compose(window);

		// display new visuals
		window.display();
	}
}
