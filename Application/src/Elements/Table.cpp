#include "Table.h"
#include "../Settings.h"
#include <fstream>
#include "../BassPlayer.h"
#include "Notification.h"

Table::Table(sf::Vector2f size, sf::Vector2f position)
{
	size_ = size;
	position_ = position;
}

void Table::handleEvents(const sf::Event& event, const sf::Vector2f& mouse)
{
	for (int i = 0; i < static_cast<int>(Settings::getNumOfFiles()); i++) {
		rowButton_[i].handleEvents(event, ng::Cursor::getPosition());
		if (rowButton_[i].isPressed()) {
			rowButton_[i].setSelectColor(pressedSelectColor_);
		} else{
			rowButton_[i].setSelectColor(selectColor_);
		}

		if (rowButton_[i].isActive()) {
			BassPlayer::setPlayingFile(i);
		}
	}
}

// this function needs settings to be loaded
void Table::create()
{
	rowButton_.clear();
	for (int i = 0; i < static_cast<int>(Settings::getNumOfFiles()); i++) {
		// -- Shape --------------
		ng::Button button;
		button.setSize({ size_.x, size_.y / Settings::getNumOfFiles() });
		button.setPosition({ position_.x, position_.y + i * button.getGlobalBounds().height });
		button.setSelectColor(selectColor_);
		button.setSelectThickness(outlineThickness_);

		FILE_STATE response = getFileState(i);

		if (response == FILE_STATE::STATE_PLAYING) {
			button.setFillColor(playingColor_);
		}
		else if (response == FILE_STATE::STATE_FOUND) {
			button.setFillColor(foundColor_);
		}
		else if (response == FILE_STATE::STATE_NOT_FOUND) {
			button.setFillColor(notFoundColor_);
		}

		// -- Text ---------------
		unsigned fontSize = static_cast<unsigned>(button.getGlobalBounds().height / 2.0F);
		button.setCharacterSize(fontSize);
		button.setFont(NG_FONT(fontTypeLocation_));
		button.setTextColor(fontColor_);
		button.setString(Settings::getFileName(i) + "  " +
			Settings::getBreakStartString(i) + " - " +
			Settings::getBreakEndString(i));

		// --- Push ---------------
		rowButton_.push_back(button);
	}

	// error
	if (rowButton_.size() != Settings::getNumOfFiles())
	{
		Notification::popup("Error!", 
			"Something went terribly wrong while creating table!\n"
			"Please contact Szoke Andras-Lorand to fix this issue!");

		ng::Main::closeWindow();
	}
}

void Table::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (int i = 0; i < static_cast<int>(Settings::getNumOfFiles()); i++)
	{
		target.draw(rowButton_[i]);
	}
	//target.draw(errorConfirmDialog_);
}

void Table::updateColors()
{
	for (int i = 0; i < static_cast<int>(Settings::getNumOfFiles()); i++)
	{
		FILE_STATE response = getFileState(i);

		if (response == FILE_STATE::STATE_PLAYING) {
			rowButton_[i].setFillColor(playingColor_);
		}
		else if (response == FILE_STATE::STATE_FOUND) {
			rowButton_[i].setFillColor(foundColor_);
		}
		else if (response == FILE_STATE::STATE_NOT_FOUND) {
			rowButton_[i].setFillColor(notFoundColor_);
		}
	}
}

Table::FILE_STATE Table::getFileState(int i)
{
	std::ifstream test(Settings::getFilePath(i));
	if (!test.good()) {
		return FILE_STATE::STATE_NOT_FOUND;
	}
	else {
		if (BassPlayer::getPlayingFileIndex() == i) {
			return FILE_STATE::STATE_PLAYING;
		}
		else {
			return FILE_STATE::STATE_FOUND;
		}
	}
}
