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

		texture_buffer.resize((int)COUNT_SPRITE);
		for (int obj_cnt = 0; obj_cnt < object_names->size(); obj_cnt++) {
			if (settings.isSpriteDebugOutputEnabled()) {
				std::cout << "Object " << (*object_names)[obj_cnt] << " sprite loading" << std::endl;
			}

			//get animation set
			std::vector<std::string> * anim_set = getFileList(path + "\\" + (*object_names)[obj_cnt]);
			std::vector<std::vector<sf::Texture>> object_textures;

			texture_buffer[is_sprite_type_exists((*object_names)[obj_cnt])].resize((int)COUNT_ANIMATION);
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
				if (is_animation_type_exists((*anim_set)[anim_cnt]) != -1 && is_sprite_type_exists((*object_names)[obj_cnt]) != -1) {
					texture_buffer[is_sprite_type_exists((*object_names)[obj_cnt])][is_animation_type_exists((*anim_set)[anim_cnt])] = animation;
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
		sprite->setPosition(position.x * window->getSize().x / 1920, position.y * window->getSize().y / 1080);
		sprite->setRotation(angle);
		sprite->setOrigin(origin.x, origin.y);
		sprite->setScale(window->getSize().x / 1920, window->getSize().y / 1080);

		object->frameIncrement();

		window->draw(*sprite);
	}

	bool processFrame(sf::RenderWindow * window, std::vector<std::vector<Object *>> * objects) {
		std::vector<Point> enemy_signs;

		for (int layer = 0; layer < objects->size(); layer++) {
			for (int i = 0; i < (*objects)[layer].size(); i++) {
				Object * object = (*objects)[layer][i];
				double radius = object->getCollisionModel()->getMaxRadius() * window->getSize().y / 1080;
				float hp = (float)object->getUnitInfo()->getHealth();
				float max_hp = (float)object->getUnitInfo()->getMaxHealth();

				if (object->getUnitInfo()->getFaction() == aggressive_faction && object->getObjectType() != bullet) {
					enemy_signs.push_back(object->getPosition());
				}

				drawObject(object, window);

				Point pos = object->getPosition() * window->getSize().y / 1080;
				hp_sign.setPosition(pos.x, pos.y);
				hp_sign.setString((int)hp ? std::to_string((int)hp) : "");
				hp_sign.setOrigin(hp_sign.getLocalBounds().width / 2, hp_sign.getLocalBounds().height / 2);
				hp_sign.setCharacterSize(30 * window->getSize().y / 1080);
				
				auto getColor = [](float ratio) {
					sf::Color color_max = sf::Color::Green;
					sf::Color color_min = sf::Color::Red;

					return sf::Color(color_min.r + ratio * (color_max.r - color_min.r), color_min.g + ratio * (color_max.g - color_min.g), color_min.b + ratio * (color_max.b - color_min.b));
				};
				sf::Vertex hp_circle[] =
				{
					sf::Vertex(sf::Vector2f(pos.x + sin(0) * radius, pos.y + cos(0) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12) * radius, pos.y + cos(PI * 2 / 12) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12) * radius, pos.y + cos(PI * 2 / 12) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 2) * radius, pos.y + cos(PI * 2 / 12 * 2) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 2) * radius, pos.y + cos(PI * 2 / 12 * 2) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 3) * radius, pos.y + cos(PI * 2 / 12 * 3) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 3) * radius, pos.y + cos(PI * 2 / 12 * 3) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 4) * radius, pos.y + cos(PI * 2 / 12 * 4) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 4) * radius, pos.y + cos(PI * 2 / 12 * 4) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 5) * radius, pos.y + cos(PI * 2 / 12 * 5) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 5) * radius, pos.y + cos(PI * 2 / 12 * 5) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 6) * radius, pos.y + cos(PI * 2 / 12 * 6) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 6) * radius, pos.y + cos(PI * 2 / 12 * 6) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 7) * radius, pos.y + cos(PI * 2 / 12 * 7) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 7) * radius, pos.y + cos(PI * 2 / 12 * 7) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 8) * radius, pos.y + cos(PI * 2 / 12 * 8) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 8) * radius, pos.y + cos(PI * 2 / 12 * 8) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 9) * radius, pos.y + cos(PI * 2 / 12 * 9) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 9) * radius, pos.y + cos(PI * 2 / 12 * 9) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 10) * radius, pos.y + cos(PI * 2 / 12 * 10) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 10) * radius, pos.y + cos(PI * 2 / 12 * 10) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 11) * radius, pos.y + cos(PI * 2 / 12 * 11) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(PI * 2 / 12 * 11) * radius, pos.y + cos(PI * 2 / 12 * 11) * radius)),
					sf::Vertex(sf::Vector2f(pos.x + sin(0) * radius, pos.y + cos(0) * radius)),
				};
				for (int i = 0; i < 24; i++) {
					hp_circle[i].color = getColor(hp / max_hp);
				}
				if (object->getObjectType() != hero && object->getObjectType() != alien_turret1 && object->getObjectType() != alien_turret2) {
					window->draw(hp_sign);
					window->draw(hp_circle, ceil(24.0 * hp / max_hp), sf::Lines);
				}
			}
		}

		for (int i = 0; i < enemy_signs.size(); i++) {
			float distance = 500, extra_distance = 35, max_distance = 5000;
			float length = (enemy_signs[i] - Point(window->getView().getCenter())).getLength() / window->getView().getSize().y * 1080;
			if (length > (distance + 25) && length < max_distance) {
				float angle = -((enemy_signs[i] - Point(window->getView().getCenter()))).getAngle();
				Point center = Point(window->getView().getCenter()) + Point(sin(-angle), cos(angle)) * distance * window->getView().getSize().y / 1080;
				Point v1 = Point() + center;
				Point v2 = (Point(-10, -10) * window->getView().getSize().y / 1080).getRotated(angle * 180 / PI) + center;
				Point v3 = (Point(0, 10) * window->getView().getSize().y / 1080).getRotated(angle * 180 / PI) + center;
				Point v4 = (Point(10, -10) * window->getView().getSize().y / 1080).getRotated(angle * 180 / PI) + center;
				sf::Vertex arrow[] = {
					sf::Vertex(sf::Vector2f(v1.x,v1.y)),
					sf::Vertex(sf::Vector2f(v2.x,v2.y)),
					sf::Vertex(sf::Vector2f(v2.x,v2.y)),
					sf::Vertex(sf::Vector2f(v3.x,v3.y)),
					sf::Vertex(sf::Vector2f(v3.x,v3.y)),
					sf::Vertex(sf::Vector2f(v4.x,v4.y)),
					sf::Vertex(sf::Vector2f(v4.x,v4.y)),
					sf::Vertex(sf::Vector2f(v1.x,v1.y))
				};
				window->draw(arrow, 8, sf::Lines);
			}
		}

		std::vector<Point> mission_objectives = rpg_profile.getCurrentCoordinatesList();

		for (int i = 0; i < mission_objectives.size(); i++) {
			float distance = 500, extra_distance = 35, max_distance = 5000;
			float length = (mission_objectives[i] - Point(window->getView().getCenter())).getLength() / window->getView().getSize().y * 1080;
			if (length > (distance + 25)) {
				float angle = -((mission_objectives[i] - Point(window->getView().getCenter()))).getAngle();
				Point center = Point(window->getView().getCenter()) + Point(sin(-angle), cos(angle)) * distance * window->getView().getSize().y / 1080;
				Point v1 = Point() + center;
				Point v2 = (Point(-15, -15) * window->getView().getSize().y / 1080).getRotated(angle * 180 / PI) + center;
				Point v3 = (Point(0, 15) * window->getView().getSize().y / 1080).getRotated(angle * 180 / PI) + center;
				Point v4 = (Point(15, -15) * window->getView().getSize().y / 1080).getRotated(angle * 180 / PI) + center;
				sf::Vertex arrow[] = {
					sf::Vertex(sf::Vector2f(v1.x,v1.y)),
					sf::Vertex(sf::Vector2f(v2.x,v2.y)),
					sf::Vertex(sf::Vector2f(v2.x,v2.y)),
					sf::Vertex(sf::Vector2f(v3.x,v3.y)),
					sf::Vertex(sf::Vector2f(v3.x,v3.y)),
					sf::Vertex(sf::Vector2f(v4.x,v4.y)),
					sf::Vertex(sf::Vector2f(v4.x,v4.y)),
					sf::Vertex(sf::Vector2f(v1.x,v1.y))
				};
				arrow->color = sf::Color(120, 100, 255);
				window->draw(arrow, 8, sf::Lines);
			}
		}

		return is_active;
	}

};