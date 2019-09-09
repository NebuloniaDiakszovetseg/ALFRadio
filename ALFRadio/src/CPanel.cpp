#include "CPanel.h"

void CPanel::Setup()
{
	background_.setPosition({ 0, 0 });
	background_.setSize({ 300, 600 });
	background_.setFillColor(sf::Color(60, 60, 60));

	dateTime_.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	dateTime_.setPosition({ 90, 38 });
	dateTime_.setCharacterSize(18);
	dateTime_.setFillColor(sf::Color(243, 96, 0));
	
	infoButton_.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("info_button.png"));
	infoButton_.setPosition({ 20, 20 });
	infoButton_.setScale({ 0.1f, 0.1f });

	infoScreen_.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("info_screen.png"));

	/*Volume*/
	volumeSlider_.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("volume.png"));
	volumeSlider_.setPosition({ 25, 120 });
	volumeSlider_.setFillColor(sf::Color(250, 130, 52));

	volumeText_.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	volumeText_.setCharacterSize(16);
	volumeText_.setFillColor(sf::Color(243, 96, 0));
	volumeText_.setString("Volume:");
	volumeText_.setPosition({ 150 - volumeText_.getGlobalBounds().width / 2, 95 });

	/*Intro-Outro Switcher*/
	introSwitcher_.setPosition({ 25, 170 });
	introSwitcher_.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("switcher.png"));
	introSwitcher_.setFillColor(sf::Color(250, 130, 52));
	introSwitcher_.setMarkColor(sf::Color(20, 20, 20));

	introText_.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	introText_.setFillColor(sf::Color(243, 96, 0));
	introText_.setString("Auto Intro/Outro");
	introText_.setCharacterSize(18);
	introText_.setPosition({ 75, 170 + introText_.getGlobalBounds().height / 2 + introText_.getLocalBounds().top });
	
	/*Randomize Switcher*/
	randomSwitcher_.setPosition({ 25, 225 });
	randomSwitcher_.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("switcher.png"));
	randomSwitcher_.setFillColor(sf::Color(250, 130, 52));
	randomSwitcher_.setMarkColor(sf::Color(20, 20, 20));

	randomText_.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	randomText_.setFillColor(sf::Color(243, 96, 0));
	randomText_.setString("Randomize NF Files");
	randomText_.setCharacterSize(18);
	randomText_.setPosition({ 75, 225 + randomText_.getGlobalBounds().height / 2 + randomText_.getLocalBounds().top });

	// Microphone activated volume dimmer switcher
	dimOnMic_.setPosition({ 25, 280 });
	dimOnMic_.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("switcher.png"));
	dimOnMic_.setFillColor(sf::Color(250, 130, 52));
	dimOnMic_.setMarkColor(sf::Color(20, 20, 20));

	dimOnMicText_.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	dimOnMicText_.setFillColor(sf::Color(243, 96, 0));
	dimOnMicText_.setString("Dim If Mic Active");
	dimOnMicText_.setCharacterSize(18);
	dimOnMicText_.setPosition({ 75, 280 + dimOnMicText_.getGlobalBounds().height / 2 + dimOnMicText_.getLocalBounds().top });

	/*Table*/
	headerShape_.setPosition({ 25, 340 });
	headerShape_.setSize({ 250, 30 });
	headerShape_.setFillColor(sf::Color(35,35,35));

	headerText_.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	headerText_.setCharacterSize(15);
	headerText_.setFillColor(sf::Color(243, 96, 0));
	NGin::offCentTxtInSh(10, headerText_, headerShape_);

	for (int i = 0; i < int(tableShape_.size()); i++) {
		tableShape_[i].setSize(headerShape_.getSize());
		tableShape_[i].setPosition({
			headerShape_.getPosition().x,
			headerShape_.getPosition().y + headerShape_.getSize().y * (i + 1)			
		});
		tableShape_[i].setFillColor(headerShape_.getFillColor());
		tableText_[i].setFont(*headerText_.getFont());
		tableText_[i].setCharacterSize(headerText_.getCharacterSize() - 1);
		tableText_[i].setFillColor(headerText_.getFillColor());
		tableText_[i].setString(Input::getFileName(i) + "   " +
			Input::getStartTime(i) + "-" + Input::getEndTime(i));
		NGin::offCentTxtInSh(20, tableText_[i], tableShape_[i]);
	}
	
	loadInOut();

	// sets up the channel volume
	volumeSlider_.setLevel(volumeValue_ / maxVolume_);
}

void CPanel::handleEvents(const sf::Event& event)
{
	volumeSlider_.select(NGin::UI::Cursor::getPosition());
	volumeSlider_.handleEvents(event);

	introSwitcher_.select(NGin::UI::Cursor::getPosition());
	introSwitcher_.handleEvents(event);

	randomSwitcher_.select(NGin::UI::Cursor::getPosition());
	randomSwitcher_.handleEvents(event);

	dimOnMic_.select(NGin::UI::Cursor::getPosition());
	dimOnMic_.handleEvents(event);

	// info-screen toggle
	if (infoButton_.getGlobalBounds().intersects({ NGin::UI::Cursor::getPosition(), {1,1} })) {
		infoButton_.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("nebulogo.png"));
		infoScreen_.setColor(sf::Color::White); // show
	}
	else {
		infoButton_.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("info_button.png"));
		infoScreen_.setColor(sf::Color::Transparent); // hide
	}

	// volume adjust
	if (volumeSlider_.getLevel() != volumeValue_ / maxVolume_) {
		volumeValue_ = volumeSlider_.getLevel() * maxVolume_;
	}
}

void CPanel::Update()
{
	dateTime_.setString(NGin::Timer::getSysString());

	for (int i = 0; i < int(tableShape_.size()); i++) {
		if (i == Input::getCurrIndex() && !Input::getFirstLoad())
		{
			tableShape_[i].setFillColor(sf::Color(252, 172, 69));
		}
		else {
			tableShape_[i].setFillColor(headerShape_.getFillColor());
		}
	}

	// log switcher stance changes
	if (introSwitcher_.hasChanged())
		if (introSwitcher_.isActive())
			NGin::Logger::log("Auto Intro-Outro -- Activated!");
		else
			NGin::Logger::log("Auto Intro-Outro -- Deactivated!");

	if (randomSwitcher_.hasChanged())
		if (randomSwitcher_.isActive())
			NGin::Logger::log("Randomize if no files found -- Activated!");
		else
			NGin::Logger::log("Randomize if no files found  -- Deactivated!");

	if (dimOnMic_.hasChanged())
		if (dimOnMic_.isActive())
			NGin::Logger::log("Dim app volume if microphone has input -- Activated!");
		else
			NGin::Logger::log("Dim app volume if microphone has input  -- Deactivated!");
}

void CPanel::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(background_);
	target.draw(dateTime_);
	target.draw(volumeSlider_);
	target.draw(volumeText_);
	target.draw(introSwitcher_);
	target.draw(introText_);
	target.draw(randomSwitcher_);
	target.draw(randomText_);
	target.draw(dimOnMic_);
	target.draw(dimOnMicText_);

	target.draw(headerShape_);
	target.draw(headerText_);

	for (int i = 0; i < int(tableShape_.size()); i++) {
		target.draw(tableShape_[i]);
		target.draw(tableText_[i]);
	}

	target.draw(infoScreen_); // only gets drawn if logo is hovered on
	target.draw(infoButton_);
}

void CPanel::playIntro(HCHANNEL& channel)
{
	BASS_ChannelStop(channel);
	channel = BASS_SampleGetChannel(introSample_, FALSE);
	introLength_ = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
	BASS_ChannelPlay(channel, FALSE);
}

bool CPanel::introStopped(const HCHANNEL& channel)
{
	return (introShouldPlay() && BASS_ChannelIsActive(channel) != BASS_ACTIVE_PLAYING
		&& introLength_ == BASS_ChannelGetLength(channel, BASS_POS_BYTE));
}

void CPanel::playOutro(HCHANNEL& channel)
{
	BASS_ChannelStop(channel);
	channel = BASS_SampleGetChannel(outroSample_, FALSE);
	outroLength_ = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
	BASS_ChannelPlay(channel, FALSE);
}

bool CPanel::outroStopped(const HCHANNEL& channel)
{
	return (introShouldPlay() && BASS_ChannelIsActive(channel) != BASS_ACTIVE_PLAYING
		&& outroLength_ == BASS_ChannelGetLength(channel, BASS_POS_BYTE));
}

void CPanel::loadInOut()
{
	introSample_ = BASS_SampleLoad(false, "input/intro.mp3", 0, 0, 1, BASS_DEVICE_MONO);
	if (BASS_ErrorGetCode() != BASS_OK)
	{
		NGin::Logger::log("Cannot open intro! - Auto intro/outro DISABLED!",
						   NGin::Logger::Severity::Warning);

		introSwitcher_.setisActive(false);
		introSwitcher_.setInactivity(true);
	}
	else {
		outroSample_ = BASS_SampleLoad(false, "input/outro.mp3", 0, 0, 1, BASS_DEVICE_MONO);
		if (BASS_ErrorGetCode() != BASS_OK)
		{
			NGin::Logger::log("Cannot open outro! - Auto intro/outro DISABLED!",
							   NGin::Logger::Severity::Warning);

			introSwitcher_.setisActive(false);
			introSwitcher_.setInactivity(true);
		}
		else {
			NGin::Logger::log("Intro/Outro loaded successfully!");
		}
	}
}
