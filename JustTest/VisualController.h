#pragma once

#include <iostream>

#include "Map.h"
#include "FileSearch.h"
#include "Settings.h"

#include <SFML\Graphics.hpp>

#include <string>
#include <iostream>

#define PI 3.14159265

class VisualController {

	std::vector<std::vector<std::vector<sf::Texture>>> texture_buffer;
	std::vector<std::vector<sf::Sprite>> sprite_buffer;

	bool is_active = true;

	sf::Text hp_sign;
	sf::Font base_font;

	void uploadTextures(std::string path) {
		if (settings.isSpriteDebugOutputEnabled()) {
			std::cout << " -- Sprites loading start -- " << std::endl;
		}

		std::vector<std::string> * object_names = getFileList(path);

		texture_buffer.resize((int)COUNT_OBJECT);
		for (int obj_cnt = 0; obj_cnt < object_names->size(); obj_cnt++) {
			if (settings.isSpriteDebugOutputEnabled()) {
				std::cout << "Object " << (*object_names)[obj_cnt] << " sprite loading" << std::endl;
			}

			//get animation set
			std::vector<std::string> * anim_set = getFileList(path + "\\" + (*object_names)[obj_cnt]);
			std::vector<std::vector<sf::Texture>> object_textures;

			texture_buffer[is_object_type_exists((*object_names)[obj_cnt])].resize((int)COUNT_ANIMATION);
			for (int anim_cnt = 0; anim_cnt < anim_set->size(); anim_cnt++) {
				if (settings.isSpriteDebugOutputEnabled()) {
					std::cout << "    >> Animation " << (*anim_set)[anim_cnt] << " is loading" << std::endl;
				}

				//get sprites set
				std::vector<std::string> * textures_set = getFileList(path + "\\" + ((*object_names)[obj_cnt] + "\\" + (*anim_set)[anim_cnt]));
				std::vector<sf::Texture> animation(textures_set->size());

				for (int i = 0; i < textures_set->size(); i++) {
					if (settings.isSpriteDebugOutputEnabled()) {
						std::cout << "        >> Sprite " << (*textures_set)[i] << " is loading" << std::endl;
					}
					std::string sprite_path = (path + "\\" + ((*object_names)[obj_cnt] + "\\" + (*anim_set)[anim_cnt] + "\\" + (*textures_set)[i])).c_str();
					animation[i].loadFromFile(sprite_path);
				}
				if (is_animation_type_exists((*anim_set)[anim_cnt]) != -1 && is_object_type_exists((*object_names)[obj_cnt]) != -1) {
					texture_buffer[is_object_type_exists((*object_names)[obj_cnt])][is_animation_type_exists((*anim_set)[anim_cnt])] = animation;
				}
			}
		}
		if (settings.isSpriteDebugOutputEnabled()) {
			std::cout << " -- Sprites loading completed -- " << std::endl << std::endl;
		}
		return;
	}

	void initSprites() {
		sprite_buffer.resize(texture_buffer.size());
		for (int obj_cnt = 0; obj_cnt < texture_buffer.size(); obj_cnt++) {
			sprite_buffer[obj_cnt].resize(texture_buffer[obj_cnt].size());
			for (int anim_cnt = 0; anim_cnt < texture_buffer[obj_cnt].size(); anim_cnt++) {
				if (texture_buffer[obj_cnt][anim_cnt].size() != 0) {
					sprite_buffer[obj_cnt][anim_cnt].setTexture(texture_buffer[obj_cnt][anim_cnt][0]);
				}
			}
		}
	}


public:

	VisualController() {
		const std::string texture_path = "sprites";

		uploadTextures(texture_path);
		initSprites();

		base_font.loadFromFile("a_Alterna.ttf");

		hp_sign.setFillColor(sf::Color::White);
		hp_sign.setOutlineColor(sf::Color::Black);
		hp_sign.setOutlineThickness(1);
		hp_sign.setCharacterSize(20);
		hp_sign.setFont(base_font);
	}

	void reset() {
		texture_buffer.clear();
		sprite_buffer.clear();

		const std::string texture_path = "sprites";

		uploadTextures(texture_path);
		initSprites();
	}

	void setActive() {
		is_active = true;
	}

	bool isActive() {
		return is_active;
	}

	void drawObject(Object * object, sf::RenderWindow * window) {
		int obj_type = object->getObjectSpriteType();
		int anim_type = object->getObjectAnimationType();
		int frame_num = object->getFrameNumber();

		Point position = object->getPosition();
		Point origin = object->getOrigin();
		double angle = object->getAngle();

		sf::Sprite * sprite = &sprite_buffer[obj_type][anim_type];

		if (texture_buffer[obj_type][anim_type].size() != 1) {
			sprite->setTexture(texture_buffer[obj_type][anim_type][frame_num % texture_buffer[obj_type][anim_type].size()]);
		}
		sprite->setPosition(position.x, position.y);
		sprite->setRotation(angle);
		sprite->setOrigin(origin.x, origin.y);

		object->frameIncrement();

		window->draw(*sprite);
	}

	bool processFrame(sf::RenderWindow * window, std::vector<std::vector<Object *>> * objects) {

		for (int layer = 0; layer < objects->size(); layer++) {
			for (int i = 0; i < (*objects)[layer].size(); i++) {
				Object * object = (*objects)[layer][i];
				double radius = object->getCollisionModel()->getMaxRadius();
				float hp = (float)object->getUnitInfo()->getHealth();
				float max_hp = (float)object->getUnitInfo()->getMaxHealth();

				drawObject(object, window);

				hp_sign.setPosition(object->getPosition().x, object->getPosition().y);
				hp_sign.setString((int)hp ? std::to_string((int)hp) : "");
				hp_sign.setOrigin(hp_sign.getLocalBounds().width / 2, hp_sign.getLocalBounds().height / 2);
				
				auto getColor = [](float ratio) {
					sf::Color color_max = sf::Color::Green;
					sf::Color color_min = sf::Color::Red;

					return sf::Color(color_min.r + ratio * (color_max.r - color_min.r), color_min.g + ratio * (color_max.g - color_min.g), color_min.b + ratio * (color_max.b - color_min.b));
				};

				sf::Vertex line[] =
				{
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(0) * radius, object->getPosition().y + cos(0) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12) * radius, object->getPosition().y + cos(PI * 2 / 12) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12) * radius, object->getPosition().y + cos(PI * 2 / 12) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 2) * radius, object->getPosition().y + cos(PI * 2 / 12 * 2) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 2) * radius, object->getPosition().y + cos(PI * 2 / 12 * 2) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 3) * radius, object->getPosition().y + cos(PI * 2 / 12 * 3) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 3) * radius, object->getPosition().y + cos(PI * 2 / 12 * 3) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 4) * radius, object->getPosition().y + cos(PI * 2 / 12 * 4) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 4) * radius, object->getPosition().y + cos(PI * 2 / 12 * 4) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 5) * radius, object->getPosition().y + cos(PI * 2 / 12 * 5) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 5) * radius, object->getPosition().y + cos(PI * 2 / 12 * 5) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 6) * radius, object->getPosition().y + cos(PI * 2 / 12 * 6) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 6) * radius, object->getPosition().y + cos(PI * 2 / 12 * 6) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 7) * radius, object->getPosition().y + cos(PI * 2 / 12 * 7) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 7) * radius, object->getPosition().y + cos(PI * 2 / 12 * 7) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 8) * radius, object->getPosition().y + cos(PI * 2 / 12 * 8) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 8) * radius, object->getPosition().y + cos(PI * 2 / 12 * 8) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 9) * radius, object->getPosition().y + cos(PI * 2 / 12 * 9) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 9) * radius, object->getPosition().y + cos(PI * 2 / 12 * 9) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 10) * radius, object->getPosition().y + cos(PI * 2 / 12 * 10) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 10) * radius, object->getPosition().y + cos(PI * 2 / 12 * 10) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 11) * radius, object->getPosition().y + cos(PI * 2 / 12 * 11) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(PI * 2 / 12 * 11) * radius, object->getPosition().y + cos(PI * 2 / 12 * 11) * radius)),
					sf::Vertex(sf::Vector2f(object->getPosition().x + sin(0) * radius, object->getPosition().y + cos(0) * radius)),
				};
				for (int i = 0; i < 24; i++) {
					line[i].color = getColor(hp / max_hp);
				}
				if (object->getObjectType() != hero && object->getObjectType() != alien_turret1) {
					window->draw(hp_sign);
					window->draw(line, ceil(24.0 * hp / max_hp), sf::Lines);
				}
			}
		}

		return is_active;
	}

};