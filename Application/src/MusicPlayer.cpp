#include "MusicPlayer.h"
#include "BassPlayer.h"

void MusicPlayer::setup()
{
	background_.setFillColor(backgroundColor_);
	background_.setSize(backgroundSize_);
	background_.setPosition(backgroundPosition_);

	playSwitcher_.setTexture(NG_TEXTURE(playSwitcherTextureLoc_));
	playSwitcher_.setScale(ng::ftovec(controlsScale_));
	playSwitcher_.setPosition({
		background_.getPosition().x + background_.getGlobalBounds().width / 2
			- playSwitcher_.getGlobalBounds().width / 2,
		controlsYPos_ });

	prevButton_.setTexture(NG_TEXTURE(prevSwitcherTextureLoc_));
	prevButton_.setScale(ng::ftovec(controlsScale_));
	prevButton_.setPosition({
		playSwitcher_.getPosition().x - controlsXOffset_ - prevButton_.getGlobalBounds().width,
		controlsYPos_ });

	nextButton_.setTexture(NG_TEXTURE(nextSwitcherTextureLoc_));
	nextButton_.setScale(ng::ftovec(controlsScale_));
	nextButton_.setPosition({
		playSwitcher_.getPosition().x + playSwitcher_.getGlobalBounds().width + controlsXOffset_ ,
		controlsYPos_ });

	seekSlider_.setTexture(NG_TEXTURE(seekSliderTextureLoc_));
	seekSlider_.setScale(ng::ftovec(seekSliderScale_));
	seekSlider_.setPosition({
		ng::getCenterOf(background_).x - seekSlider_.getGlobalBounds().width / 2,
		seekSliderYPos_
	});

	totalTime_.setFont(NG_FONT(fontTypeLoc_));
	totalTime_.setFillColor(fontColor_);
	totalTime_.setCharacterSize(fontSize_);

	currentTime_.setFont(NG_FONT(fontTypeLoc_));
	currentTime_.setFillColor(fontColor_);
	currentTime_.setCharacterSize(fontSize_);
}

void MusicPlayer::handleEvents(const sf::Event& event)
{
	playSwitcher_.handleEvents(event, ng::Cursor::getPosition());
	prevButton_.handleEvents(event, ng::Cursor::getPosition());
	nextButton_.handleEvents(event, ng::Cursor::getPosition());
	seekSlider_.handleEvents(event, ng::Cursor::getPosition());

	if (seekSlider_.hasChanged())
		BassPlayer::setPercentagePlayed(seekSlider_.getLevel());

	if (playSwitcher_.hasChanged()) {
		if (playSwitcher_.isActive()) BassPlayer::play();
		else BassPlayer::pause();
	}
	
	if (prevButton_.isActive()) BassPlayer::prev();
	
	if (nextButton_.isActive()) BassPlayer::next();
}

void MusicPlayer::update()
{
	// update time-related texts
	totalTime_.setString(BassPlayer::getLengthString());
	totalTime_.setPosition({
		seekSlider_.getPosition().x + seekSlider_.getGlobalBounds().width
		- totalTime_.getGlobalBounds().width - timeTextOffset_.x,
		seekSlider_.getPosition().y - totalTime_.getGlobalBounds().height
		- timeTextOffset_.y
	});

	currentTime_.setString(BassPlayer::getCurrentPositionString());
	currentTime_.setPosition({
		seekSlider_.getPosition().x + timeTextOffset_.x,
		seekSlider_.getPosition().y - totalTime_.getGlobalBounds().height
		- timeTextOffset_.y
	});

	// update slider to percentage played
	if (BassPlayer::getPercentagePlayed() <= 1.0F)
		seekSlider_.setLevel(BassPlayer::getPercentagePlayed());

	// disable switcher if playback ended
	if (BassPlayer::playbackHasEnded())
		playSwitcher_.setIsActive(false);

	// start if start time reached
	if (startedForTime != ng::Timer::getSysHMStr()) {
		for (int i = 0; i < static_cast<int>(Settings::getNumOfFiles()); i++) {
			if (ng::Timer::getSys().tm_hour == Settings::getBreakStartHour(i) &&
				ng::Timer::getSys().tm_min == Settings::getBreakStartMinute(i))
			{
				startedForTime = ng::Timer::getSysHMStr();
				BassPlayer::setPlayingFileIndex(i);
				BassPlayer::play();
				playSwitcher_.setIsActive(true);
			}
		}
	}

	// force end if end time reached
	if (endedForTime != ng::Timer::getSysHMStr()) {
		int index = BassPlayer::getPlayingFileIndex();
		if (0 <= index && index < static_cast<int>(Settings::getNumOfFiles()) &&
			ng::Timer::getSys().tm_hour == Settings::getBreakEndHour(index) &&
			ng::Timer::getSys().tm_min == Settings::getBreakEndMinute(index))
		{
			endedForTime = ng::Timer::getSysHMStr();
			BassPlayer::pause();
			playSwitcher_.setIsActive(false);
		}
	}
}

void MusicPlayer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(background_);
	target.draw(playSwitcher_);
	target.draw(prevButton_);
	target.draw(nextButton_);
	target.draw(seekSlider_);
	target.draw(currentTime_);
	target.draw(totalTime_);
}
