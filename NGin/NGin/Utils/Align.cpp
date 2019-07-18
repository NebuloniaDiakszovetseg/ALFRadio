#include "Align.h"

namespace NGin {
	void centCentTxtInBou(sf::Text& text, const sf::FloatRect& bounds) {
		// skip empty text avoiding errors
		if (text.getString() != "") {
			text.setPosition({ bounds.left + (bounds.width / 2) - (text.getGlobalBounds().width / 2),
			bounds.top + (bounds.height / 2) - (text.getGlobalBounds().height / 2 + text.getLocalBounds().top / 2)});
		}
	}
	void centCentTxtInSh(sf::Text& text, const sf::RectangleShape& shape)
	{
		// corrects for outline misposition
		if (shape.getOutlineThickness() > 0) {
			centCentTxtInBou(text, {
			shape.getGlobalBounds().left,
			shape.getGlobalBounds().top,
			shape.getGlobalBounds().width - shape.getOutlineThickness() ,
			shape.getGlobalBounds().height - shape.getOutlineThickness() });
		}
		else centCentTxtInBou(text, shape.getGlobalBounds());
	}
	void offCentTxtInBou(float x_offset, sf::Text& text, const sf::FloatRect& bounds)
	{
		// skip empty text avoiding errors
		if (text.getString() != "") {
			text.setPosition({ bounds.left + x_offset,
			bounds.top + (bounds.height / 2) - (text.getGlobalBounds().height / 2 + text.getLocalBounds().top / 2 ) });
		}
	}
	void offCentTxtInSh(float x_offset, sf::Text& text, const sf::RectangleShape& shape)
	{
		// corrects for outline misposition
		if (shape.getOutlineThickness() > 0) {
			offCentTxtInBou(x_offset, text, {
			shape.getGlobalBounds().left,
			shape.getGlobalBounds().top,
			shape.getGlobalBounds().width - shape.getOutlineThickness() ,
			shape.getGlobalBounds().height - shape.getOutlineThickness() });
		}
		else offCentTxtInBou(x_offset, text, shape.getGlobalBounds());
	}
}
