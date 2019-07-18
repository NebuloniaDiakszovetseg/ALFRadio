#pragma once
#include "SFML/Graphics.hpp"

namespace NGin {
	// horizontally center vertically center text in bounds
	void centCentTxtInBou(sf::Text& text, const sf::FloatRect& bounds);
	// horizontally center vertically center text in shape
	void centCentTxtInSh(sf::Text& text, const sf::RectangleShape& shape);
	// horizontally offset vertically center text in bounds
	void offCentTxtInBou(float x_offset, sf::Text& text, const sf::FloatRect& bounds);
	// horizontally offset vertically center text in shape
	void offCentTxtInSh(float x_offset, sf::Text& text, const sf::RectangleShape& shape);
}

