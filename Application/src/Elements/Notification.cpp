#include "Notification.h"

sf::Vector2f Notification::size_ = { 775.0F, 250.0F };
sf::Color Notification::bgColor_ = { 60, 60, 60 };
float Notification::textYOffset_ = -30;
unsigned Notification::fontSize_ = 30;

std::string Notification::buttonTextureLoc_ = "button.png";
std::string Notification::fontTypeLocation_ = "arial_font.ttf";
sf::Color Notification::fontColor_{ 243, 96, 0 };

ng::ConfirmDialog Notification::confirmDialog_{
	ng::ConfirmDialog::DIALOG_TYPE::DIALOG_CLOSE_ONLY };

void Notification::setup()
{
	confirmDialog_.setSize(size_);
	confirmDialog_.setShapeColor(bgColor_);
	confirmDialog_.setFont(NG_FONT(fontTypeLocation_));
	confirmDialog_.setAllTextColor(fontColor_);
	confirmDialog_.setCharacterSize(fontSize_);
	confirmDialog_.setButtonTexture(NG_TEXTURE(buttonTextureLoc_));
	confirmDialog_.setTextYOffset(textYOffset_);
}

void Notification::popup(std::string windowName, std::string message)
{
	confirmDialog_.setString(message);
	confirmDialog_.setIsActive(true);
	confirmDialog_.drawInWindow(windowName);
}
