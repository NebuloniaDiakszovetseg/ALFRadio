#pragma once
#include "NGin.h"

class Notification {
public:
	static void setup();

	// Warning! Contains a while cicle which blocks
	// input in other windows
	static void popup(std::string windowName, std::string message);

private:
	static sf::Vector2f size_;
	static sf::Color bgColor_;
	static float textYOffset_;
	static unsigned fontSize_;

	static std::string buttonTextureLoc_;
	static std::string fontTypeLocation_;
	static sf::Color fontColor_;

	static ng::ConfirmDialog confirmDialog_;
};