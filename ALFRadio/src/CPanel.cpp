#include "CPanel.h"

void CPanel::Setup()
{
	background.setPosition({ 0, 0 });
	background.setSize({ 300, 600 });
	background.setFillColor(sf::Color(60, 60, 60));

	date_time.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	date_time.setPosition({ 90, 38 });
	date_time.setCharacterSize(18);
	date_time.setFillColor(sf::Color(243, 96, 0));
	
	info_button.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("info.png"));
	info_button.setPosition({ 20, 20 });
	info_button.setScale({ 0.1f, 0.1f });

	/*Volume*/
	volume.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("volume.png"));
	volume.setPosition({ 25, 120 });
	volume.setFillColor(sf::Color(250, 130, 52));

	vol_text.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	vol_text.setCharacterSize(16);
	vol_text.setFillColor(sf::Color(243, 96, 0));
	vol_text.setString("Volume:");
	vol_text.setPosition({ 150 - vol_text.getGlobalBounds().width / 2, 95 });

	/*Intro-Outro Switcher*/
	inoutro.setPosition({ 25, 170 });
	inoutro.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("switcher.png"));
	inoutro.setFillColor(sf::Color(250, 130, 52));
	inoutro.setMarkColor(sf::Color(20, 20, 20));

	inout_txt.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	inout_txt.setFillColor(sf::Color(243, 96, 0));
	inout_txt.setString("Auto Intro/Outro");
	inout_txt.setCharacterSize(18);
	inout_txt.setPosition({ 75, 170 + inout_txt.getGlobalBounds().height / 2 + inout_txt.getLocalBounds().top });
	
	/*Randomize Switcher*/
	rand_sw.setPosition({ 25, 225 });
	rand_sw.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("switcher.png"));
	rand_sw.setFillColor(sf::Color(250, 130, 52));
	rand_sw.setMarkColor(sf::Color(20, 20, 20));

	rand_txt.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	rand_txt.setFillColor(sf::Color(243, 96, 0));
	rand_txt.setString("Randomize NF Files");
	rand_txt.setCharacterSize(18);
	rand_txt.setPosition({ 75, 225 + rand_txt.getGlobalBounds().height / 2 + rand_txt.getLocalBounds().top });

	/*Table*/
	header_elem.setPosition({ 25, 350 });
	header_elem.setSize({ 250, 30 });
	header_elem.setFillColor(sf::Color(35,35,35));

	header_txt.setFont(*NGin::ResourceCodex::Acquire<sf::Font>("KeepCalm-Medium.ttf"));
	header_txt.setCharacterSize(15);
	header_txt.setFillColor(sf::Color(243, 96, 0));

	for (int i = 0; i < int(table_elem.size()); i++) {
		table_elem[i].setSize(header_elem.getSize());
		table_elem[i].setPosition({
			header_elem.getPosition().x,
			header_elem.getPosition().y + header_elem.getSize().y * (i + 1)			
		});
		table_elem[i].setFillColor(header_elem.getFillColor());
		table_txt[i].setFont(*header_txt.getFont());
		table_txt[i].setCharacterSize(header_txt.getCharacterSize() - 1);
		table_txt[i].setFillColor(header_txt.getFillColor());
		table_txt[i].setString(Input::getFileName(i) + "   " +
			Input::getStartTime(i) + "-" + Input::getEndTime(i));
		NGin::offCentTxtInSh(20, table_txt[i], table_elem[i]);
	}
	
	loadInOut();

	// sets up the channel volume
	volume.setLevel(volume_val / max_volume);
}

void CPanel::handleEvents(const sf::Event& event)
{
	volume.select(NGin::UI::Cursor::getPosition());
	volume.handleEvents(event);

	inoutro.select(NGin::UI::Cursor::getPosition());
	inoutro.handleEvents(event);

	rand_sw.select(NGin::UI::Cursor::getPosition());
	rand_sw.handleEvents(event);

	// info-screen toggle
	if (info_button.getGlobalBounds().intersects({ NGin::UI::Cursor::getPosition(), {1,1} })) {
		info_button.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("nebulogo.png"));
		info_screen.setColor(sf::Color::White); // show
	}
	else {
		info_button.setTexture(*NGin::ResourceCodex::Acquire<sf::Texture>("info.png"));
		info_screen.setColor(sf::Color::Transparent); // hide
	}

	// volume adjust
	if (volume.getLevel() != volume_val / max_volume) {
		volume_val = volume.getLevel() * max_volume;
	}
}

void CPanel::Update()
{
	date_time.setString(NGin::Timer::getSysString());
	header_txt.setString(NGin::Timer::getSysYMDStr());
	NGin::offCentTxtInSh(10, header_txt, header_elem);

	for (int i = 0; i < int(table_elem.size()); i++) {
		if (i == Input::getCurrIndex())
		{
			table_elem[i].setFillColor(sf::Color(252, 172, 69));
		}
		else {
			table_elem[i].setFillColor(header_elem.getFillColor());
		}
	}

	// log switcher stance changes
	if (inoutro.hasChanged()) {
		if (inoutro.isActive()) {
			NGin::Logger::log("Auto Intro-Outro -- Activated!");
		}
		else {
			NGin::Logger::log("Auto Intro-Outro -- Deactivated!");
		}
	}
}

void CPanel::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(background);
	target.draw(date_time);
	target.draw(volume);
	target.draw(vol_text);
	target.draw(inoutro);
	target.draw(inout_txt);
	target.draw(rand_sw);
	target.draw(rand_txt);
	target.draw(info_screen); // only gets drawn if logo is hovered on
	target.draw(info_button);

	target.draw(header_elem);
	target.draw(header_txt);

	for (int i = 0; i < int(table_elem.size()); i++) {
		target.draw(table_elem[i]);
		target.draw(table_txt[i]);
	}
}

void CPanel::playIntro(HCHANNEL& channel)
{
	BASS_ChannelStop(channel);
	channel = BASS_SampleGetChannel(intro, FALSE);
	intro_length = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
	BASS_ChannelPlay(channel, FALSE);
}

bool CPanel::introStopped(const HCHANNEL& channel)
{
	return (inoutActive() && BASS_ChannelIsActive(channel) != BASS_ACTIVE_PLAYING
		&& intro_length == BASS_ChannelGetLength(channel, BASS_POS_BYTE));
}

void CPanel::playOutro(HCHANNEL& channel)
{
	BASS_ChannelStop(channel);
	channel = BASS_SampleGetChannel(outro, FALSE);
	outro_length = BASS_ChannelGetLength(channel, BASS_POS_BYTE);
	BASS_ChannelPlay(channel, FALSE);
}

bool CPanel::outroStopped(const HCHANNEL& channel)
{
	return (inoutActive() && BASS_ChannelIsActive(channel) != BASS_ACTIVE_PLAYING
		&& outro_length == BASS_ChannelGetLength(channel, BASS_POS_BYTE));
}

void CPanel::loadInOut()
{
	intro = BASS_SampleLoad(false, "input/intro.mp3", 0, 0, 1, BASS_DEVICE_STEREO);
	if (BASS_ErrorGetCode() != BASS_OK) {
		NGin::Logger::log("Cannot open intro! - Auto intro/outro DISABLED!", NGin::Logger::Severity::Warning);
		inoutro.setisActive(false);
		inoutro.setInactivity(true);
	}
	else {
		outro = BASS_SampleLoad(false, "input/outro.mp3", 0, 0, 1, BASS_DEVICE_STEREO);
		if (BASS_ErrorGetCode() != BASS_OK) {
			NGin::Logger::log("Cannot open outro! - Auto intro/outro DISABLED!", NGin::Logger::Severity::Warning);
			inoutro.setisActive(false);
			inoutro.setInactivity(true);
		}
		else {
			NGin::Logger::log("Intro/Outro loaded successfully!");
		}
	}
}
