#pragma once

#include "../Utils/SetFormatString.h"
#include "PlayerRPGProfile.h"

class MissionVisualController {
	sf::Font main_font;
	sf::Text name_text;
	sf::Text description_text;

	int base_character_size = 30;

	int mission_list_border = 80;
	int mission_list_width = 400;
	int mission_list_textbox_height = 75;

	float current_shift = 0;

public:

	MissionVisualController(sf::RenderWindow & window) {
		main_font.loadFromFile(consts.getDefaultFontPath());

		name_text.setPosition(sf::Vector2f(window.getView().getSize().x / 2, 150 * window.getView().getSize().y / 1080));
		name_text.setFillColor(sf::Color::White);
		name_text.setOutlineColor(sf::Color::Black);
		name_text.setOutlineThickness(1);
		name_text.setCharacterSize(base_character_size * window.getView().getSize().y / 1080);
		name_text.setFont(main_font);

		description_text.setPosition(sf::Vector2f(window.getView().getSize().x / 2, 150 * window.getView().getSize().y / 1080));
		description_text.setFillColor(sf::Color::White);
		description_text.setOutlineColor(sf::Color::Black);
		description_text.setOutlineThickness(1);
		description_text.setCharacterSize(base_character_size * window.getView().getSize().y / 1080);
		description_text.setFont(main_font);
	}

	void processMissionList(sf::RenderWindow & window, int selected_mission) {
        auto buffer = phrase_container.getPhraseBuffer(L"no_miss_sign_GUI", 0);
		std::vector<Mission> mission_list = rpg_profile.getMissionList();
		sf::Vector2f view_size = window.getView().getSize();
		sf::Vector2f view_center = sf::Vector2f(view_size.x / 2, view_size.y / 2);

		if (mission_list.size() == 0) {
			setFormatString(buffer[buffer.size() * rand() / (RAND_MAX + 1)], name_text, mission_list_width, mission_list_textbox_height, base_character_size, window);
            name_text.setFillColor(sf::Color::White);
			name_text.setPosition(window.getView().getCenter() - view_center + sf::Vector2f(mission_list_border, mission_list_border));
			window.draw(name_text);
            buffer = phrase_container.getPhraseBuffer(L"no_miss_broad_sign_GUI", 0);
			setFormatString(buffer[buffer.size() * rand() / (RAND_MAX + 1)], description_text, 1920 - mission_list_width - 2 * mission_list_border, 1080 - 2 * mission_list_border, base_character_size, window);
			description_text.setPosition(window.getView().getCenter() - view_center + sf::Vector2f(mission_list_width + 2 * mission_list_border, mission_list_border));
			window.draw(description_text);
		}

		if (selected_mission < 0 || selected_mission >= mission_list.size()) {
			return;
		}

		for (int i = 0; i < mission_list.size(); i++) {
			setFormatString((i == selected_mission ? L">" : L"") + mission_list[i].getShortDescription(), name_text, mission_list_width, mission_list_textbox_height, base_character_size, window);
			current_shift = current_shift + 0.01 * (selected_mission - current_shift);
			name_text.setPosition(window.getView().getCenter() - sf::Vector2f(view_size.x / 2, 0) + sf::Vector2f(mission_list_border, (i - current_shift) * (mission_list_textbox_height)));
			if (rpg_profile.getCurrentMissionNumber() == i) {
				name_text.setFillColor(sf::Color(120, 100, 255));
			}
			else {
				name_text.setFillColor(sf::Color::White);
			}
			window.draw(name_text);
		}

		setFormatString(mission_list[selected_mission].getBroadDescription(), description_text, 1920 - mission_list_width - 2 * mission_list_border, 1080 - 2 * mission_list_border, base_character_size, window);
		description_text.setPosition(window.getView().getCenter() - view_center + sf::Vector2f(mission_list_width + 2 * mission_list_border, mission_list_border));
		window.draw(description_text);

		description_text.setPosition(window.getView().getCenter() - view_center + sf::Vector2f(mission_list_width + 2 * mission_list_border, mission_list_border + description_text.getGlobalBounds().height + base_character_size));
		buffer = phrase_container.getPhraseBuffer(L"reward_sign_GUI", 0);
        setFormatString(buffer[buffer.size() * rand() / (RAND_MAX + 1)] + L": " + std::to_wstring((int)mission_list[selected_mission].getReward()), description_text, 1920 - mission_list_width - 2 * mission_list_border, 1080 - 2 * mission_list_border, base_character_size, window);
		window.draw(description_text);
	}

	void processCompletedMissionList(sf::RenderWindow & window, int selected_mission) {
        auto buffer = phrase_container.getPhraseBuffer(L"compl_no_miss_sign_GUI", 0);
		std::vector<Mission> mission_list = rpg_profile.getCompletedMissionList();
		sf::Vector2f view_size = window.getView().getSize();
		sf::Vector2f view_center = sf::Vector2f(view_size.x / 2, view_size.y / 2);

		if (mission_list.size() == 0) {
			setFormatString(buffer[buffer.size() * rand() / (RAND_MAX + 1)], name_text, mission_list_width, mission_list_textbox_height, base_character_size, window);
            name_text.setFillColor(sf::Color::White);
			name_text.setPosition(window.getView().getCenter() - view_center + sf::Vector2f(mission_list_border, mission_list_border));
			window.draw(name_text); 
            buffer = phrase_container.getPhraseBuffer(L"compl_no_miss_broad_sign_GUI", 0);
			setFormatString(buffer[buffer.size() * rand() / (RAND_MAX + 1)], description_text, 1920 - mission_list_width - 2 * mission_list_border, 1080 - 2 * mission_list_border, base_character_size, window);
			description_text.setPosition(window.getView().getCenter() - view_center + sf::Vector2f(mission_list_width + 2 * mission_list_border, mission_list_border));
			window.draw(description_text);
		}

		if (selected_mission < 0 || selected_mission >= mission_list.size()) {
			return;
		}

		for (int i = 0; i < mission_list.size(); i++) {
			setFormatString((i == selected_mission ? L">" : L"") + mission_list[i].getShortDescription(), name_text, mission_list_width, mission_list_textbox_height, base_character_size, window);
			current_shift = current_shift + 0.01 * (selected_mission - current_shift);
			name_text.setPosition(window.getView().getCenter() - sf::Vector2f(view_size.x / 2, 0) + sf::Vector2f(mission_list_border, (i - current_shift) * (mission_list_textbox_height)));
			name_text.setFillColor(sf::Color::White);
			window.draw(name_text);
		}

		setFormatString(mission_list[selected_mission].getBroadDescription(), description_text, 1920 - mission_list_width - 2 * mission_list_border, 1080 - 2 * mission_list_border, base_character_size, window);
		description_text.setPosition(window.getView().getCenter() - view_center + sf::Vector2f(mission_list_width + 2 * mission_list_border, mission_list_border));
		window.draw(description_text);

		description_text.setPosition(window.getView().getCenter() - view_center + sf::Vector2f(mission_list_width + 2 * mission_list_border, mission_list_border + description_text.getGlobalBounds().height + base_character_size));
		buffer = phrase_container.getPhraseBuffer(L"reward_sign_GUI", 0);
        setFormatString(buffer[buffer.size() * rand() / (RAND_MAX + 1)] + L": " + std::to_wstring((int)mission_list[selected_mission].getReward()), description_text, 1920 - mission_list_width - 2 * mission_list_border, 1080 - 2 * mission_list_border, base_character_size, window);
		window.draw(description_text);
	}
};
