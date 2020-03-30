#include "SettingsUI.h"
#include "BassPlayer.h"

void SettingsUI::setup()
{
	banner_.setTexture(&NG_TEXTURE(bannerTextureLoc_));
	banner_.setSize(backgroundSize_);
	banner_.setPosition(backgroundPosition_);

	background_.setSize(backgroundSize_);
	background_.setPosition(backgroundPosition_);
	background_.setFillColor(backgroundColor_);

	settingsSwitcher_.setSize(settingsSwitcherSize_);
	settingsSwitcher_.setTexture(NG_TEXTURE(settingsTextureLoc_));
	settingsSwitcher_.setPosition(settingsSwitcherPos_);

	winResDropdown_.setTexture(NG_TEXTURE(dropdownTextureLoc_));
	winResDropdown_.setFont(NG_FONT(fontTypeLocation_));
	winResDropdown_.setTextColor(fontColor_);
	winResDropdown_.setScale(dropdownScale_);
	winResDropdown_.setPosition(winResDropdownPos_);
	winResDropdown_.setCharacterSize(fontSize_);

	winResText_.setFont(NG_FONT(fontTypeLocation_));
	winResText_.setCharacterSize(fontSize_);
	winResText_.setFillColor(fontColor_);
	winResText_.setString(winResString_);
	ng::centerTextInBounds(winResText_,
		winResDropdown_.getClosedGlobalBounds(), textYOffset_);

	conResDropdown_.setTexture(NG_TEXTURE(dropdownTextureLoc_));
	conResDropdown_.setFont(NG_FONT(fontTypeLocation_));
	conResDropdown_.setTextColor(fontColor_);
	conResDropdown_.setScale(dropdownScale_);
	conResDropdown_.setPosition(conResDropdownPos_);
	conResDropdown_.setCharacterSize(fontSize_);

	conResText_.setFont(NG_FONT(fontTypeLocation_));
	conResText_.setCharacterSize(fontSize_);
	conResText_.setFillColor(fontColor_);
	conResText_.setString(conResString_);
	ng::centerTextInBounds(conResText_,
		conResDropdown_.getClosedGlobalBounds(), textYOffset_);

	if (!ng::Console::isActive())
	{
		conResDropdown_.setDisabled(true);
		conResDropdown_.setFillColor(disabledElementColor_);
		conResDropdown_.setTextColor(disabledElementColor_);
		conResText_.setFillColor(disabledElementColor_);
	}

	windowTypeDropdown_.setTexture(NG_TEXTURE(dropdownTextureLoc_));
	windowTypeDropdown_.setFont(NG_FONT(fontTypeLocation_));
	windowTypeDropdown_.setTextColor(fontColor_);
	windowTypeDropdown_.setScale(dropdownScale_);
	windowTypeDropdown_.setPosition(windowTypeDropdownPos_);
	windowTypeDropdown_.setCharacterSize(fontSize_);

	windowTypeText_.setFont(NG_FONT(fontTypeLocation_));
	windowTypeText_.setCharacterSize(fontSize_);
	windowTypeText_.setFillColor(fontColor_);
	windowTypeText_.setString(windowTypeString_);
	ng::centerTextInBounds(windowTypeText_,
		windowTypeDropdown_.getClosedGlobalBounds(), textYOffset_);

	applyButton_.setPosition(applyButtonPosition_);
	applyButton_.setScale(applyButtonScale_);
	applyButton_.setTexture(NG_TEXTURE(buttonTextureLoc_));
	applyButton_.setFont(NG_FONT(fontTypeLocation_));
	applyButton_.setTextColor(fontColor_);
	applyButton_.setCharacterSize(fontSize_);

	maxVolumeText_.setFont(NG_FONT(fontTypeLocation_));
	maxVolumeText_.setCharacterSize(fontSize_);
	maxVolumeText_.setFillColor(fontColor_);

	maxVolumeSlider_.setTexture(NG_TEXTURE(sliderLocation_));
	maxVolumeSlider_.setScale(ng::ftovec(sliderScale_));
	maxVolumeSlider_.setPosition(maxVolumeSliderPosition_);
	maxVolumeSlider_.setLevel(Settings::getMaximumVolume());

	dimThresholdText_.setFont(NG_FONT(fontTypeLocation_));
	dimThresholdText_.setCharacterSize(fontSize_);
	dimThresholdText_.setFillColor(fontColor_);

	dimThresholdSlider_.setTexture(NG_TEXTURE(sliderLocation_));
	dimThresholdSlider_.setScale(ng::ftovec(sliderScale_));
	dimThresholdSlider_.setPosition(dimThresholdSliderPosition_);
	dimThresholdSlider_.setLevel(Settings::getDimThreshold());

	dimVolumeText_.setFont(NG_FONT(fontTypeLocation_));
	dimVolumeText_.setCharacterSize(fontSize_);
	dimVolumeText_.setFillColor(fontColor_);

	dimVolumeSlider_.setTexture(NG_TEXTURE(sliderLocation_));
	dimVolumeSlider_.setScale(ng::ftovec(sliderScale_));
	dimVolumeSlider_.setPosition(dimVolumeSliderPosition_);
	dimVolumeSlider_.setLevel(Settings::getDimVolume());

	micMaxValueRect_.setFillColor(disabledElementColor_);
	micMaxValueRect_.setSize(micMaxValueRectSize_);
	micMaxValueRect_.setPosition(micMaxValueRectPos_);
	micValueRect_.setFillColor(fontColor_);
	micValueRect_.setPosition(micMaxValueRect_.getPosition());

	confirmDialog_.setShapeColor(confirmDialogBgColor_);
	confirmDialog_.setFont(NG_FONT(fontTypeLocation_));
	confirmDialog_.setAllTextColor(fontColor_);
	confirmDialog_.setCharacterSize(fontSize_);
	confirmDialog_.setButtonTexture(NG_TEXTURE(buttonTextureLoc_));
	confirmDialog_.setButtonScale(confirmDialogButtonScale_);
	confirmDialog_.setButtonOffset(confirmDialogButtonOffset_);
	confirmDialog_.setTextYOffset(confirmDialogTextYOffset_);

	for (int i = 0; i < static_cast<int>(windowTypes_.size()); i++) {
		windowTypeDropdown_.addDropString(windowTypes_[i]);
	}

	if (ng::Main::windowType == ng::WINDOW_TYPE::WINDOW_UNRESIZEABLE)
		windowTypeDropdown_.setActiveDrop(1); // windowed
	else if (ng::Main::windowType == ng::WINDOW_TYPE::WINDOW_BORDERLESS)
		windowTypeDropdown_.setActiveDrop(2); // borderless
	else if (ng::Main::windowType == ng::WINDOW_TYPE::WINDOW_FULLSCREEN)
		windowTypeDropdown_.setActiveDrop(3); // fullscreen
	else if (ng::Main::windowType == ng::WINDOW_TYPE::WINDOW_RESIZEABLE)
		NG_LOG_ERROR("Window has unsupported Type!"); /// !!! Resizeable window is considered error

	const std::string winResNowString =
		std::to_string(getWindowWidth()) + " x " + std::to_string(getWindowHeight());
	winResDropdown_.setDropString(0, winResNowString);

	const std::string conResNowString =
		std::to_string(getConsoleWidth()) + " x " + std::to_string(getConsoleHeight());
	conResDropdown_.setDropString(0, conResNowString);

	for (int i = 0; i < static_cast<int>(resolutions_.size()); i++) {
		if (resolutions_[i].x <= sf::VideoMode::getDesktopMode().width)
		{
			const std::string resolutionString =
				std::to_string(resolutions_[i].x) + " x " + std::to_string(resolutions_[i].y);
			winResDropdown_.addDropString(resolutionString);
			conResDropdown_.addDropString(resolutionString);
		}
		else {
			resolutions_.erase(resolutions_.begin() + i); // erase unavailable resolutions
			i--; // i needs to step back due to erased resolution
		}
	}

	saveWindowType_ = windowTypes_[windowTypeDropdown_.getActiveDrop() - 1];
	saveWinRes_ = { ng::Main::windowVideoMode.width,
					ng::Main::windowVideoMode.height };
	saveConRes_ = ng::Console::getSize();
}

void SettingsUI::handleEvents(const sf::Event& event)
{
	settingsSwitcher_.handleEvents(event, ng::Cursor::getPosition());
	confirmDialog_.handleEvents(event, ng::Cursor::getPosition());

	if (settingsSwitcher_.isActive() && !confirmDialog_.isActive())
	{
		conResDropdown_.handleEvents(event, ng::Cursor::getPosition());
		winResDropdown_.handleEvents(event, ng::Cursor::getPosition());
		windowTypeDropdown_.handleEvents(event, ng::Cursor::getPosition());
		maxVolumeSlider_.handleEvents(event, ng::Cursor::getPosition());
		dimThresholdSlider_.handleEvents(event, ng::Cursor::getPosition());
		dimVolumeSlider_.handleEvents(event, ng::Cursor::getPosition());
		applyButton_.handleEvents(event, ng::Cursor::getPosition());

		// adjust volume string to percentage of volume
		std::string volStr = maxVolumeString_ +
			std::to_string(static_cast<int>(maxVolumeSlider_.getLevel() * 100)) + "%";

		maxVolumeText_.setString(volStr);

		ng::centerTextInBounds(maxVolumeText_,
			maxVolumeSlider_.getGlobalBounds(), textYOffset_);

		// adjust dim threshold string to actual dim threshold
		std::string thresholdStr = dimThresholdString_ +
			std::to_string(static_cast<int>(dimThresholdSlider_.getLevel() * 100)) + "%";

		dimThresholdText_.setString(thresholdStr);

		ng::centerTextInBounds(dimThresholdText_,
			dimThresholdSlider_.getGlobalBounds(), textYOffset_);

		// adjust dim volume string to current dim volue value
		std::string dimVolStr = dimVolumeString_ +
			std::to_string(static_cast<int>(dimVolumeSlider_.getLevel() * 100)) + "%";

		dimVolumeText_.setString(dimVolStr);

		ng::centerTextInBounds(dimVolumeText_,
			dimVolumeSlider_.getGlobalBounds(), textYOffset_);

		// adjust file values after slider values
		jsonFile_[appVolumeKey] = maxVolumeSlider_.getLevel();
		jsonFile_[dimVolumeKey] = dimVolumeSlider_.getLevel();
		jsonFile_[dimThresholdKey] = dimThresholdSlider_.getLevel();
	}

	if (applyButton_.isActive()) {
		applyNewSettings(CHANGE::CHANGE_TEMPORARY);
	}
	else if (confirmDialog_.getResponse() == ng::ConfirmDialog::RESPONSE::RESPONSE_OK) {
		applyNewSettings(CHANGE::CHANGE_PERMANENT);
	}
	else if (confirmDialog_.getResponse() == ng::ConfirmDialog::RESPONSE::RESPONSE_CLOSE) {
		applyNewSettings(CHANGE::CHANGE_REVERT);
	}

	if (windowTypeDropdown_.getActiveDrop() - 1 == fullScreenIndex_) {
		winResDropdown_.setDisabled(true);
	}
	else {
		winResDropdown_.setDisabled(false);
	}
}

void SettingsUI::update()
{
	if (confirmDialog_.isActive()) {
		resetTimer_ += ng::Timer::getDeltaTime();

		confirmDialog_.setString(confirmDialogString_ +
			std::to_string(static_cast<int>(resetLimit_ - resetTimer_)));

		if (resetTimer_ > resetLimit_) {
			resetTimer_ = 0;
			applyNewSettings(CHANGE::CHANGE_REVERT);
		}
	}

	if (!getDimIsActive()) {
		dimThresholdSlider_.setDisabled(true);
		dimVolumeSlider_.setDisabled(true);
		micValueRect_.setSize({ 0, micMaxValueRectSize_.y });

		dimThresholdSlider_.setFillColor(disabledElementColor_);
		dimVolumeSlider_.setFillColor(disabledElementColor_);
		dimThresholdText_.setFillColor(disabledElementColor_);
		dimVolumeText_.setFillColor(disabledElementColor_);
	}
	else {
		if (BassPlayer::getMicrophoneLevel() > 0.0F) {
			micValueRect_.setSize({
				micMaxValueRectSize_.x * BassPlayer::getMicrophoneLevel(),
				micMaxValueRectSize_.y
				});
		}

		dimThresholdSlider_.setDisabled(false);
		dimVolumeSlider_.setDisabled(false);

		dimThresholdSlider_.setFillColor({ 255, 255, 255 });
		dimVolumeSlider_.setFillColor({ 255, 255, 255 });
		dimThresholdText_.setFillColor(fontColor_);
		dimVolumeText_.setFillColor(fontColor_);
	}


}

void SettingsUI::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (settingsSwitcher_.isActive()) {
		target.draw(background_);

		target.draw(maxVolumeSlider_);
		target.draw(maxVolumeText_);

		target.draw(dimThresholdSlider_);
		target.draw(dimThresholdText_);

		target.draw(dimVolumeSlider_);
		target.draw(dimVolumeText_);

		target.draw(micMaxValueRect_);
		target.draw(micValueRect_);

		target.draw(conResDropdown_);
		target.draw(conResText_);

		target.draw(winResDropdown_);
		target.draw(winResText_);

		target.draw(windowTypeDropdown_);
		target.draw(windowTypeText_);

		target.draw(applyButton_);
	}
	else {
		target.draw(banner_);
	}

	target.draw(settingsSwitcher_);
	target.draw(confirmDialog_);
}

void SettingsUI::applyNewSettings(const CHANGE changeType)
{
	sf::Vector2u winRes;
	sf::Vector2u conRes;
	std::string windowTypeString;

	// --- Change based on changeType_ ------------------------------------------
	if (changeType == CHANGE::CHANGE_REVERT)
	{
		conRes = saveConRes_;
		winRes = saveWinRes_;
		windowTypeString = saveWindowType_;
		confirmDialog_.setIsActive(false);
	}
	// PERMANENT always runs after TEMPORARY -> very important !
	else if (changeType == CHANGE::CHANGE_PERMANENT)
	{
		saveWinRes_ = { ng::Main::windowVideoMode.width,
						ng::Main::windowVideoMode.height };
		saveConRes_ = ng::Console::getSize();
		saveWindowType_ = windowTypes_[windowTypeDropdown_.getActiveDrop() - 1];

		conRes = saveConRes_;
		winRes = saveWinRes_;
		windowTypeString = saveWindowType_;

		jsonFile_[renderWindowKey][widthKey] = winRes.x;
		jsonFile_[renderWindowKey][heightKey] = winRes.y;
		jsonFile_[renderWindowKey][typeKey] = static_cast<int>(ng::Main::windowType);
		jsonFile_[consoleWindowKey][widthKey] = conRes.x;
		jsonFile_[consoleWindowKey][heightKey] = conRes.y;

		confirmDialog_.setIsActive(false);
	}
	else if (changeType == CHANGE::CHANGE_TEMPORARY)
	{
		windowTypeString = windowTypes_[windowTypeDropdown_.getActiveDrop() - 1];

		if (windowTypeString == windowTypes_[fullScreenIndex_]) // fullscreen resolution
			winRes = { sf::VideoMode::getDesktopMode().width,
					   sf::VideoMode::getDesktopMode().height };
		else if (winResDropdown_.getActiveDrop() != 0)  // if dropdown has one drop selected
			winRes = resolutions_[winResDropdown_.getActiveDrop() - 1]; // apply selected
		else
			winRes = { ng::Main::windowVideoMode.width,
					   ng::Main::windowVideoMode.height }; // else use default

		if (conResDropdown_.getActiveDrop() != 0)  // if dropdown has one drop selected
			conRes = resolutions_[conResDropdown_.getActiveDrop() - 1]; // apply selected
		else
			conRes = ng::Console::getSize(); // else use default
	}
	// --------------------------------------------------------------------------

	// --- Apply changes internally and externally ------------------------------
	// check if there is need for a dialog box
	auto tempConRes = ng::Console::getSize();
	auto tempWinVideoMode = ng::Main::windowVideoMode;
	auto tempType = ng::Main::windowType;

	if (windowTypeString == windowTypes_[0]) // windowed
	{
		ng::Main::windowType = ng::WINDOW_TYPE::WINDOW_UNRESIZEABLE;
		windowTypeDropdown_.setActiveDrop(1);
	}
	else if (windowTypeString == windowTypes_[1]) // borderless
	{
		ng::Main::windowType = ng::WINDOW_TYPE::WINDOW_BORDERLESS;
		windowTypeDropdown_.setActiveDrop(2);
	}
	else if (windowTypeString == windowTypes_[2]) // fullscreen
	{
		ng::Main::windowType = ng::WINDOW_TYPE::WINDOW_FULLSCREEN;
		windowTypeDropdown_.setActiveDrop(3);
	}

	ng::Main::windowVideoMode = { winRes.x , winRes.y };
	ng::Console::setSize(conRes);

	std::string winResString =
		std::to_string(winRes.x) + " x " + std::to_string(winRes.y);
	winResDropdown_.setDropString(0, winResString);

	std::string conResString =
		std::to_string(conRes.x) + " x " + std::to_string(conRes.y);
	conResDropdown_.setDropString(0, conResString);

	// only start dialog if changes have been made
	if (changeType == CHANGE::CHANGE_TEMPORARY &&
		(tempType != ng::Main::windowType ||
			tempWinVideoMode != ng::Main::windowVideoMode ||
			tempConRes != ng::Console::getSize()))
	{
		confirmDialog_.setIsActive(true); // open dialog
		resetTimer_ = 0; // start resetTimer_ from 0
	}
	// --------------------------------------------------------------------------
}
