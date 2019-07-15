#include "InfoScreen.h"

void InfoScreen::show()
{
	should_draw = true;
}

void InfoScreen::hide()
{
	should_draw = false;
}

void InfoScreen::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (should_draw) {
		target.draw(background);
	}
}
