#pragma once

#include <iostream>

#include "Research.h"
#include "FileSearch.h"
#include "Settings.h"
#include "Tutorial.h"

#include <SFML/Graphics.hpp>

#include <string>
#include <iostream>


class ResearchVisualController {

	std::vector<std::vector<sf::Texture>> texture_buffer;
	std::vector<sf::Sprite> sprite_buffer;

	sf::Font font;
	sf::Text cost_sign, description, tutorial_sign;

	bool is_active = true;

	void uploadTextures(std::string path) {
		if (settings.isSpriteDebugOutputEnabled()) {
			std::cout << " -- Research sprites loading start -- " << std::endl;
		}

		std::vector<std::string> * research_names = getFileList(path);

		texture_buffer.resize(research_manager.getResearchName().size());
		for (int res_cnt = 0; res_cnt < research_names->size(); res_cnt++) {
			if (settings.isSpriteDebugOutputEnabled()) {
				std::cout << "Research " << (*research_names)[res_cnt] << " sprite loading" << std::endl;
			}

			sf::Texture active, notactive, unlocked;
			active.loadFromFile(path + path_separator + (*research_names)[res_cnt] + path_separator + "active.png");
			notactive.loadFromFile(path + path_separator + (*research_names)[res_cnt] + path_separator + "notactive.png");
			unlocked.loadFromFile(path + path_separator + (*research_names)[res_cnt] + path_separator + "unlocked.png");

			int res_id = research_manager.stringToResearch(stringToWstring((*research_names)[res_cnt]));

			texture_buffer[res_id].push_back(notactive);
			texture_buffer[res_id].push_back(active);
			texture_buffer[res_id].push_back(unlocked);
		}
		if (settings.isSpriteDebugOutputEnabled()) {
			std::cout << " -- Research sprites loading completed -- " << std::endl << std::endl;
		}
		return;
	}

	void initSprites() {
		sprite_buffer.resize(texture_buffer.size());
		for (int res_cnt = 0; res_cnt < texture_buffer.size(); res_cnt++) {
			sprite_buffer[res_cnt].setTexture(texture_buffer[res_cnt][0]);
		}
	}

	void drawResearchInfo(sf::RenderWindow * window, sf::Vector2f viewport_pos, int cur_research_index) {
		cost_sign.setString("Research points: " + std::to_string((int)resource_manager.getResearch()) +"\nCost: " + std::to_string((int)research_manager.getResearch((ResearchList)cur_research_index)->cost) + " research points");
		cost_sign.setOrigin(-viewport_pos);
		cost_sign.setCharacterSize(40 * window->getView().getSize().y / 1080);

		auto tmp = research_manager.getResearchNode((ResearchList)cur_research_index);
		std::vector<std::wstring> buffer = phrase_container.getPhraseBuffer(tmp->description_id);
		description.setString(buffer[rand() * buffer.size() / (RAND_MAX + 1)]);

		description.setOrigin(-viewport_pos + sf::Vector2f(0, description.getGlobalBounds().height));
		description.setCharacterSize(40 * window->getView().getSize().y / 1080);

		window->draw(cost_sign);
		window->draw(description);
	}

public:

	ResearchVisualController(sf::RenderWindow * window) {
		const std::string texture_path = "research_sprites";

		uploadTextures(texture_path);
		initSprites();

		font.loadFromFile(consts.getDefaultFontPath());

		cost_sign.setPosition(sf::Vector2f(-(int)window->getSize().x / 2 + 100, -(int)window->getSize().y / 2 + 100));
		cost_sign.setFillColor(sf::Color::White);
		cost_sign.setOutlineColor(sf::Color::Black);
		cost_sign.setOutlineThickness(1);
		cost_sign.setCharacterSize(40);
		cost_sign.setFont(font);

		description.setPosition(sf::Vector2f(-(int)window->getSize().x / 2 + 100, (int)window->getSize().y / 2 - 100));
		description.setFillColor(sf::Color::White);
		description.setOutlineColor(sf::Color::Black);
		description.setOutlineThickness(1);
		description.setCharacterSize(40);
		description.setFont(font);

		tutorial_sign.setPosition(sf::Vector2f((int)window->getSize().x / 8, -(int)window->getSize().y / 2 + 70 * (int)window->getSize().y / 1080));
		tutorial_sign.setFillColor(sf::Color::White);
		tutorial_sign.setOutlineColor(sf::Color::Black);
		tutorial_sign.setOutlineThickness(1);
		tutorial_sign.setCharacterSize(40);
		tutorial_sign.setFont(font);
	}

	void setActive() {
		is_active = true;
	}

	bool isActive() {
		return is_active;
	}

	void drawResearch(ResearchNode * res_node, sf::RenderWindow * window) {
		int obj_type = (int)res_node->type;

		Point position = res_node->pos * window->getView().getSize().y / 1080;
		Point origin = Point(texture_buffer[obj_type][0].getSize().x / 2, texture_buffer[obj_type][0].getSize().y / 2);

		sf::Sprite * sprite = &sprite_buffer[obj_type];

		sprite->setPosition(position.x, position.y);
		Point lu_corner = research_manager.getLeftUpGraphCorner();
		Point graph_size = research_manager.getGraphSize();

		sprite->setScale(sf::Vector2f(consts.getResearchIconSize() / sprite->getTexture()->getSize().x * window->getView().getSize().y / 1080, consts.getResearchIconSize() / sprite->getTexture()->getSize().y * window->getView().getSize().y / 1080));
		sprite->setOrigin((consts.getResearchIconSize()) / sprite->getScale().x / 2 * window->getView().getSize().y / 1080, (consts.getResearchIconSize()) / sprite->getScale().y / 2 * window->getView().getSize().y / 1080);
		
		window->draw(*sprite);
	}

	bool processFrame(sf::RenderWindow * window, sf::Vector2f viewport_pos, int cur_research_index) {
		int res_amount = research_manager.getResearchAmount();
		for (int i = 0; i < res_amount; i++) {   // draw connections between nodes
			ResearchNode * node = research_manager.getResearchNode(i);
			for (int par_num = 0; par_num < node->parents.size(); par_num++) {
				ResearchNode * par_node = research_manager.getResearchNode(node->parents[par_num]);
				sf::Vertex line[] =
				{
					sf::Vertex(sf::Vector2f(node->pos.x * window->getView().getSize().y / 1080, node->pos.y * window->getView().getSize().y / 1080)),
					sf::Vertex(sf::Vector2f(par_node->pos.x * window->getView().getSize().y / 1080, par_node->pos.y * window->getView().getSize().y / 1080))
				};
				line->color = sf::Color::White;

				window->draw(line, 2, sf::Lines);
			}
		}
		for (int i = 0; i < res_amount; i++) {
			sprite_buffer[i].setTexture(texture_buffer[i][research_manager.isResearchActive(i)]);
			if (research_manager.isResearchUnlocked(i)) {
				sprite_buffer[i].setTexture(texture_buffer[i][2]);
			}
			drawResearch(research_manager.getResearchNode(i), window);
		}
		drawResearchInfo(window, viewport_pos, cur_research_index);

		if (tutorial.getCurrentStep() == tutorial.research_tutorial_close) {
			tutorial_sign.setString(phrase_container.getPhraseBuffer(L"tutorial_research_close", 0)[0]/*"Try to research something by selecting it\nwith WASD on left stick and pressing\nSpace on keyboard or A on gamepad\nand then exit with F"*/);
			tutorial_sign.setOrigin(-viewport_pos + sf::Vector2f(tutorial_sign.getLocalBounds().width / 2, 0));
			tutorial_sign.setCharacterSize(40 * window->getView().getSize().y / 1080);
		}
		else {
			tutorial_sign.setString("");
		}
		window->draw(tutorial_sign);

		return is_active;
	}

};