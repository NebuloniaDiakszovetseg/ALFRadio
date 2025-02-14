#include "ControlPanel.h"
#include "BassPlayer.h"
#include "RandomList.h"
#include "SettingsUI.h"

void ControlPanel::setup()
{
	background_.setFillColor(backgroundColor_);
	background_.setSize(backgroundSize_);
	background_.setPosition(backgroundPosition_);

	dateTime_.setFont(NG_FONT(fontTypeLocation_));
	dateTime_.setCharacterSize(fontSize_);
	dateTime_.setFillColor(fontColor_);
	dateTime_.setPosition(dateTimePos_);
	
	infoHoverOn_.setTexture(&NG_TEXTURE(infoTexture1Location_));
	infoHoverOn_.setSize(infoSize_);
	infoHoverOn_.setPosition(infoPosition_);
	infoScreen_.setTexture(NG_TEXTURE(infoScreenTextureLoc_));
	settingsInfoScreen_.setTexture(NG_TEXTURE(settingsInfoScreenTextureLoc_));
	infoToggle_ = false;

	volumeSlider_.setTexture(NG_TEXTURE(volumeSliderLoc_));
	volumeSlider_.setScale(ng::ftovec(volumeSliderScale_));
	volumeSlider_.setPosition({ 
		ng::getCenterOf(background_).x - volumeSlider_.getGlobalBounds().width / 2,
		volumeSliderPositionY_});
	volumeSlider_.setFillColor(elementColor_);
	volumeSlider_.setLevel(DEFAULT_VOLUME_VALUE);
	volumeSlider_.setSelectThickness(volumeSliderSelectThickness_);

	volumeText_.setFont(NG_FONT(fontTypeLocation_));
	volumeText_.setCharacterSize(fontSize_);
	volumeText_.setFillColor(fontColor_);

	introSwitcher_.setTexture(NG_TEXTURE(switcherTextureLocation_));
	introSwitcher_.setPosition(introSwitcherPosition_);
	introSwitcher_.setFillColor(elementColor_);
	introSwitcher_.setMarkColor(switcherMarkColor_);
	introSwitcher_.setScale(ng::ftovec(switcherScale_));

	introSwitcherText_.setPosition(introSwitcherTextPos_);
	introSwitcherText_.setString(introSwitcherString_);
	introSwitcherText_.setFont(NG_FONT(fontTypeLocation_));
	introSwitcherText_.setCharacterSize(fontSize_);
	introSwitcherText_.setFillColor(fontColor_);

	if (!BassPlayer::introIsLoaded() || !BassPlayer::outroIsLoaded())
	{
		NG_LOG_WARN("Auto intro and outro disabled due to input files not found!");
		NG_LOG_WARN("Intro Location: ", Settings::getIntroLocation());
		NG_LOG_WARN("Outro Location: ", Settings::getOutroLocation());
		introSwitcher_.setDisabled(true);
		introSwitcher_.setButtonColor(disabledElementColor_);
		introSwitcherText_.setFillColor(disabledElementColor_);
	}
	else {
		introSwitcher_.setIsActive(Settings::getAutoIntroOutro());
	}

	notFoundSwitcher_.setTexture(NG_TEXTURE(switcherTextureLocation_));
	notFoundSwitcher_.setPosition(notFoundSwitcherPosition_);
	notFoundSwitcher_.setButtonColor(elementColor_);
	notFoundSwitcher_.setMarkColor(switcherMarkColor_);
	notFoundSwitcher_.setScale(ng::ftovec(switcherScale_));

	notFoundSwitcherText_.setPosition(notFoundSwitcherTextPos_);
	notFoundSwitcherText_.setString(notFoundSwitcherString_);
	notFoundSwitcherText_.setFont(NG_FONT(fontTypeLocation_));
	notFoundSwitcherText_.setCharacterSize(fontSize_);
	notFoundSwitcherText_.setFillColor(fontColor_);

	if (RandomList::noInput())
	{
		NG_LOG_WARN("Randomize if files not found disabled due to"
		"bad or no input!");

		notFoundSwitcher_.setDisabled(true);
		notFoundSwitcher_.setButtonColor(disabledElementColor_);
		notFoundSwitcherText_.setFillColor(disabledElementColor_);
	}
	else {
		notFoundSwitcher_.setIsActive(Settings::getFNFIsEnabled());
	}

	dimSwitcher_.setTexture(NG_TEXTURE(switcherTextureLocation_));
	dimSwitcher_.setPosition(dimSwitcherPosition_);
	dimSwitcher_.setMarkColor(switcherMarkColor_);
	dimSwitcher_.setButtonColor(elementColor_);
	dimSwitcher_.setScale(ng::ftovec(switcherScale_));
	dimSwitcher_.setIsActive(Settings::getDimIsActive());

	dimSwitcherText_.setPosition(dimSwitcherTextPos_);
	dimSwitcherText_.setString(dimSwitcherString_);
	dimSwitcherText_.setFont(NG_FONT(fontTypeLocation_));
	dimSwitcherText_.setCharacterSize(fontSize_);
	dimSwitcherText_.setFillColor(fontColor_);
	dimTimer_ = Settings::getDimDuration();

	// update table based on files read/playing
	table_.create();
}

void ControlPanel::handleEvents(const sf::Event& event)
{
	volumeSlider_.handleEvents(event, ng::Cursor::getPosition());
	introSwitcher_.handleEvents(event, ng::Cursor::getPosition());
	notFoundSwitcher_.handleEvents(event, ng::Cursor::getPosition());
	dimSwitcher_.handleEvents(event, ng::Cursor::getPosition());
	table_.handleEvents(event, ng::Cursor::getPosition());

	if (infoHoverOn_.getGlobalBounds().contains(ng::Cursor::getPosition()))
	{
		infoHoverOn_.setTexture(&NG_TEXTURE(infoTexture2Location_));
		infoToggle_ = true;
	}
	else {
		infoHoverOn_.setTexture(&NG_TEXTURE(infoTexture1Location_));
		infoToggle_ = false;
	}
}

void ControlPanel::update()
{
	Settings::setDimIsActive(dimSwitcher_.isActive());
	Settings::setAutoIntroOutro(introSwitcher_.isActive());
	Settings::setFNFIsEnabled(notFoundSwitcher_.isActive());

	// adjust date and time display when time passes by
	dateTime_.setString(ng::Timer::getSysString());

	// update table colors to fit reality
	table_.updateColors();

	// error logging handled by BassPlayer
	if(BassPlayer::microphoneWorks() && dimSwitcher_.isDisabled())
	{
		dimSwitcher_.setDisabled(false);
		dimSwitcher_.setButtonColor(elementColor_);
		dimSwitcherText_.setFillColor(fontColor_);
	}
	else if(!BassPlayer::microphoneWorks() && !dimSwitcher_.isDisabled()) {
		dimSwitcher_.setDisabled(true);
		dimSwitcher_.setButtonColor(disabledElementColor_);
		dimSwitcherText_.setFillColor(disabledElementColor_);
	}

	// if dimming enabled
	if (Settings::getDimIsActive() && BassPlayer::microphoneWorks())
	{
		// if threshold exceeded
		if (BassPlayer::getMicrophoneLevel() > Settings::getDimThreshold())
		{
			dimTimer_ = 0.0F;
			if (volumeBackup_ == -1) volumeBackup_ = volumeSlider_.getLevel();
			volumeSlider_.setLevel(volumeBackup_ * Settings::getDimVolume());
			volumeSlider_.setDisabled(true);
		}

		dimTimer_ += ng::Timer::getDeltaTime();
		if (dimTimer_ > Settings::getDimDuration() && volumeBackup_ != -1)
		{
			volumeSlider_.setLevel(volumeBackup_);
			volumeSlider_.setDisabled(false);
			volumeBackup_ = -1;
			dimTimer_ = 0.0F;
		}
	}
	// restore volume after dim deactivated
	else if (volumeBackup_ != -1) {
		volumeSlider_.setLevel(volumeBackup_);
		volumeSlider_.setDisabled(false);
		volumeBackup_ = -1;
	}

	// adjust volume string to percentage of volume
	std::string volTxt = volumeString_
		+ std::to_string(int(volumeSlider_.getLevel() * 100)) + "%";
	volumeText_.setString(volTxt); // set
	ng::centerTextInShape(volumeText_, background_, volumeTextOffsetY_); // center

	BassPlayer::setVolume(Settings::getMaximumVolume() * volumeSlider_.getLevel());
}

void ControlPanel::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(background_);
	
	target.draw(dateTime_);
	
	target.draw(volumeText_);
	target.draw(volumeSlider_);
	
	target.draw(introSwitcher_);
	target.draw(introSwitcherText_);
	
	target.draw(notFoundSwitcher_);
	target.draw(notFoundSwitcherText_);
	
	target.draw(dimSwitcher_);
	target.draw(dimSwitcherText_);

	// Table
	target.draw(table_);

	if (infoToggle_) {
		target.draw(infoScreen_);
		if (SettingsUI::isActive()) {
			target.draw(settingsInfoScreen_);
		}
	}

	target.draw(infoHoverOn_);
}
