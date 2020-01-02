#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "GeometryComponents.h"
#include "Settings.h"
#include "FileSearch.h"

#include <SFML\Graphics.hpp>

class BackgroundManager {
public:
	enum BackgroundElementType {
		star,

		BACK_ELEM_COUNT
	};

	struct Element {
		Point pos;
		BackgroundElementType type;
		unsigned int frame_shift;

		Element(){
			frame_shift = rand();
		}
		
		Element(Point pos, BackgroundElementType type) : pos(pos), type(type) {
			frame_shift = rand();
		}
	};

	struct Layer {
		std::vector<Element> list;
		float move_coef = 0.1, min_range = 0, new_elem_amount = 5;

	};

private:

	// non-drawable info

	std::vector<std::string> background_elem_names = {
		"star",    // star
	};

	std::vector<Layer> layers;
	Point last_shift;
	float shift_threshold = 100, generation_range = 2500, delete_range = 3000, max_angle = PI / 3;

	// drawable info

	std::vector<std::vector<sf::Texture>> texture_buffer;
	std::vector<sf::Sprite> sprite_buffer;

	bool is_background_name(std::string example) {
		for (int i = 0; i < BACK_ELEM_COUNT; i++) {
			if (background_elem_names[i] == example) {
				return i;
			}
		}
		return -1;
	}

	void refreshMap(Point center) {
		last_shift.normalize();
		if (last_shift.getLength() == 0) {
			return;
		}

		for (int layer = 0; layer < layers.size(); layer++) {
			for (int i = 0; i < layers[layer].list.size(); i++) {
				if ((layers[layer].list[i].pos - center).getLength() > delete_range) {
					layers[layer].list.erase(layers[layer].list.begin() + i);
				}
			}

			for (int new_elem_count = 0; new_elem_count < layers[layer].new_elem_amount; new_elem_count++) {
				Point try_point;
				while (true) {
					float angle = fixAngle((rand() / (float)RAND_MAX - 0.5) * max_angle + last_shift.getAngle());
					try_point = Point(sin(angle), cos(angle)) * generation_range + center;

					bool in_min_range = false;
					for (int layer = 0; layer < layers.size(); layer++) {
						for (int i = 0; i < layers[layer].list.size(); i++) {
							if ((layers[layer].list[i].pos - try_point).getLength() < layers[layer].min_range) {
								in_min_range = true;
							}
						}
					}

					if (!in_min_range) {
						break;
					}
				}
				layers[layer].list.push_back(Element(try_point, (BackgroundElementType)layer));
			}
		}

		last_shift = Point();
	}

	void moveBackground(Point shift) {
		for (int layer = 0; layer < layers.size(); layer++) {
			for (int i = 0; i < layers[layer].list.size(); i++) {
				layers[layer].list[i].pos += shift * layers[layer].move_coef;
			}
		}
	}


	void uploadTextures(std::string path) {
		if (settings.isSpriteDebugOutputEnabled()) {
			std::cout << " -- Background sprites loading start -- " << std::endl;
		}

		std::vector<std::string> * object_names = getFileList(path);

		texture_buffer.resize((int)BACK_ELEM_COUNT);
		for (int obj_cnt = 0; obj_cnt < object_names->size(); obj_cnt++) {

			//get animation set
			std::vector<std::string> * textures_set = getFileList(path + "\\" + (*object_names)[obj_cnt]);
			std::vector<sf::Texture> animation(textures_set->size());

			for (int i = 0; i < textures_set->size(); i++) {
				std::string sprite_path = (path + "\\" + ((*object_names)[obj_cnt] + "\\" + (*textures_set)[i])).c_str();
				animation[i].loadFromFile(sprite_path);
			}

			if (is_background_name((*object_names)[obj_cnt]) != -1) {
				texture_buffer[is_background_name((*object_names)[obj_cnt])] = animation;
			}
		}
		if (settings.isSpriteDebugOutputEnabled()) {
			std::cout << " -- Background sprites loading completed -- " << std::endl << std::endl;
		}
		return;
	}

	void initSprites() {
		sprite_buffer.resize(texture_buffer.size());
		for (int obj_cnt = 0; obj_cnt < texture_buffer.size(); obj_cnt++) {
			if (texture_buffer[obj_cnt].size() != 0) {
				sprite_buffer[obj_cnt].setTexture(texture_buffer[obj_cnt][0]);
			}
		}
	}

public:

	BackgroundManager() {
		layers.resize(BACK_ELEM_COUNT);

		uploadTextures("background_components");
		initSprites();
	}

	void processFrame(Point pos_shift, Point center) {
		last_shift += pos_shift;
		moveBackground(pos_shift);
		if (last_shift.getLength() > shift_threshold) {
			refreshMap(center);
		}
	}

	void draw(sf::RenderWindow & window) {
		for (int layer = 0; layer < layers.size(); layer++) {
			for (int i = 0; i < layers[layer].list.size(); i++) {
				Point pos = layers[layer].list[i].pos;
				sprite_buffer[layer].setTexture(texture_buffer[layer][layers[layer].list[i].frame_shift % texture_buffer[layer].size()]);
				sprite_buffer[layer].setPosition(pos.x * window.getSize().x / 1920, pos.y * window.getSize().y / 1080);

				layers[layer].list[i].frame_shift++;
				window.draw(sprite_buffer[layer]);
			}
		}
	}

} background_manager;