#pragma once

#include <iostream>

#include "Map.h"
#include "FileSearch.h"
#include "Settings.h"

#include <SFML\Graphics.hpp>

#include <string>
#include <iostream>


class VisualController {

	std::vector<std::vector<std::vector<sf::Texture>>> texture_buffer;
	std::vector<std::vector<sf::Sprite>> sprite_buffer;

	bool is_active = true;

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

				drawObject(object, window);
			}
		}

		return is_active;
	}

};