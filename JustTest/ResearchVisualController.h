#pragma once

#include <iostream>

#include "Research.h"
#include "FileSearch.h"
#include "Settings.h"

#include <SFML\Graphics.hpp>

#include <string>
#include <iostream>


class ResearchVisualController {

	std::vector<std::vector<sf::Texture>> texture_buffer;
	std::vector<sf::Sprite> sprite_buffer;

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
			active.loadFromFile(path + "\\" + (*research_names)[res_cnt] + "\\" + "active.png");
			notactive.loadFromFile(path + "\\" + (*research_names)[res_cnt] + "\\" + "notactive.png");
			unlocked.loadFromFile(path + "\\" + (*research_names)[res_cnt] + "\\" + "unlocked.png");

			texture_buffer[res_cnt].push_back(notactive);
			texture_buffer[res_cnt].push_back(active);
			texture_buffer[res_cnt].push_back(unlocked);
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


public:

	ResearchVisualController() {
		const std::string texture_path = "research_sprites";

		uploadTextures(texture_path);
		initSprites();
	}

	void setActive() {
		is_active = true;
	}

	bool isActive() {
		return is_active;
	}

	void drawResearch(ResearchNode * res_node, sf::RenderWindow * window) {
		int obj_type = (int)res_node->type;

		Point position = res_node->pos;
		Point origin = Point(texture_buffer[obj_type][0].getSize().x / 2, texture_buffer[obj_type][0].getSize().y / 2);

		sf::Sprite * sprite = &sprite_buffer[obj_type];

		sprite->setPosition(position.x, position.y);
		Point lu_corner = research_manager.getLeftUpGraphCorner();
		Point graph_size = research_manager.getGraphSize();

		sprite->setScale(sf::Vector2f(consts.getResearchIconSize() / sprite->getTexture()->getSize().x, consts.getResearchIconSize() / sprite->getTexture()->getSize().y));
		sprite->setOrigin((consts.getResearchIconSize()) / sprite->getScale().x / 2, (consts.getResearchIconSize()) / sprite->getScale().y / 2);
		
		window->draw(*sprite);
	}

	bool processFrame(sf::RenderWindow * window) {
		int res_amount = research_manager.getResearchAmount();
		for (int i = 0; i < res_amount; i++) {   // draw connections between nodes
			ResearchNode * node = research_manager.getResearchNode(i);
			for (int par_num = 0; par_num < node->parents.size(); par_num++) {
				ResearchNode * par_node = research_manager.getResearchNode(node->parents[par_num]);
				sf::Vertex line[] =
				{
					sf::Vertex(sf::Vector2f(node->pos.x, node->pos.y)),
					sf::Vertex(sf::Vector2f(par_node->pos.x, par_node->pos.y))
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

		return is_active;
	}

};