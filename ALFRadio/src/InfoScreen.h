#pragma once
#include "SFML/Graphics.hpp"

class InfoScreen : public sf::Drawable {
public:
	InfoScreen() {
		background.setFillColor(sf::Color(0, 0, 0, 200));
		background.setSize({1000, 600});
	}

	void show();
	void hide();

	void draw(sf::RenderTarget& target, sf::RenderStates states) const;
private:
	sf::RectangleShape background;
	
	bool should_draw = false;
};