#include "Application.h"

Application::Application()
{
	NG_LOG_INFO("SFML Version: ", SFML_VERSION_MAJOR, ".",
		".", SFML_VERSION_MINOR, ".", SFML_VERSION_PATCH);

	NG_LOG_INFO("BASS Version: ", BassPlayer::bassVersionText());

	NG_LOG_INFO("NGin Version: ", NG_VERSION_MAJOR,
		".", NG_VERSION_MINOR, ".", NG_VERSION_PATCH);

	NG_LOG_NOTE("App by Szoke Andras-Lorand");

	ng::Main::setWindowIcon(windowIconString_);
	ng::Main::window.setFramerateLimit(frameRateLimit_);

	Notification::setup();

	Settings::load();
	RandomList::load();

	if (Settings::getConsoleIsDisabled()) // hides console if set
		ShowWindow(GetConsoleWindow(), SW_HIDE);

	ng::Main::windowVideoMode = 
		sf::VideoMode{Settings::getWindowWidth(), Settings::getWindowHeight()};
	ng::Main::windowType = ng::WINDOW_TYPE(Settings::getWindowType());
	ng::Main::windowName = WIN_NAME;

	ng::Console::setSize({
		Settings::getConsoleWidth(),
		Settings::getConsoleHeight() });
	ng::Console::setName(CON_NAME);

	ng::Main::view = sf::View{ {0, 0, VIEW_WIDTH, VIEW_HEIGHT} };

	BassPlayer::setup();

	settingsUI_.setup();
	musicPlayer_.setup();
	controlPanel_.setup();
}

Application::~Application()
{
	Settings::save();
	BassPlayer::close();
}

void Application::handleEvents()
{
	settingsUI_.handleEvents(event_);
	controlPanel_.handleEvents(event_);
	musicPlayer_.handleEvents(event_);
}

void Application::update()
{
	RandomList::update();

	settingsUI_.update();
	controlPanel_.update();
	musicPlayer_.update();

	BassPlayer::update();
}

void Application::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(musicPlayer_);
	target.draw(settingsUI_);
	target.draw(controlPanel_);
}
