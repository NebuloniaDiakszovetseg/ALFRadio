#pragma once
#include "NGin.h"

class Table: public sf::Drawable {
public:
	Table(sf::Vector2f size, sf::Vector2f position);

	void handleEvents(const sf::Event& event, const sf::Vector2f& mouse);

	// update table colors
	void updateColors();

	// refreshRate -- seconds between each refresh
	void create();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
	// -- General Properties ----------
	sf::Vector2f position_; // position of the table(top-left)
	sf::Vector2f size_; // size of the whole table

	const std::string buttonTextureLoc_ = "button.png";
	const std::string fontTypeLocation_ = "arial_font.ttf";
	const sf::Color fontColor_{ 243, 96, 0 };

	// --- Buttons ---------------------------
	std::vector<ng::Button> rowButton_;
	sf::Color pressedSelectColor_{ 243, 96, 0 };
	sf::Color selectColor_{ 255,255,255 };
	const float outlineThickness_ = -2.5F;

	// --- Error confirm dialog -------------
	const sf::Vector2f confirmDialogSize_{ 775.0F, 250.0F };
	const sf::Color confirmDialogBgColor_ = { 60, 60, 60 };
	const float confirmDialogTextYOffset_ = -30;
	const unsigned confirmDialogFontSize_ = 30;

	ng::ConfirmDialog confirmDialog_{
		ng::ConfirmDialog::DIALOG_TYPE::DIALOG_CLOSE_ONLY};

	// --- File States -----------------------
	enum class FILE_STATE {
		STATE_NOT_FOUND = 0, // red
		STATE_FOUND, // green
		STATE_PLAYING // orange
	};
	sf::Color notFoundColor_{ 128,35,35 };
	sf::Color foundColor_{ 35,35,35 };
	sf::Color playingColor_{ 80, 80, 80 };
	// get the state of file with given index
	Table::FILE_STATE getFileState(int i);
};