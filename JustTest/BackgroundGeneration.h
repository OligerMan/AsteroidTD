#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "GeometryComponents.h"
#include "Settings.h"
#include "FileSearch.h"
#include "FPS.h"

#include <SFML/Graphics.hpp>

class BackgroundManager {
public:

	struct Element {
		Point pos;
        Point speed;
		int type;
		unsigned int frame_shift;

		Element(){
			frame_shift = rand();
		}
		
		Element(Point pos, int type, Point speed) : pos(pos), type(type), speed(speed) {
			frame_shift = rand();
		}
	};

	struct Layer {
		std::vector<Element> list;
        float move_coef = 0.1, min_range = 0, new_elem_amount = 5, max_speed = 1, shift_threshold = 200;
        Point last_shift;
	};

private:

	// non-drawable info

    std::vector<std::string> background_elem_names;

	std::vector<Layer> layers;
	float generation_range = 5200, delete_range = 5500, max_angle = PI;

	// drawable info

	std::vector<std::vector<sf::Texture>> texture_buffer;
	std::vector<sf::Sprite> sprite_buffer;

	int is_background_name(std::string example) {
		for (int i = 0; i < background_elem_names.size(); i++) {
			if (background_elem_names[i] == example) {
				return i;
			}
		}
		return -1;
	}

	void refreshMap(int layer_num) {
        if (layer_num < 0 && layer_num >= layers.size()) {
            return;
        }
		layers[layer_num].last_shift.normalize();
		if (layers[layer_num].last_shift.getLength() == 0) {
			return;
		}

        for (int i = 0; i < layers[layer_num].list.size(); i++) {
            if ((layers[layer_num].list[i].pos).getLength() > delete_range) {
                layers[layer_num].list.erase(layers[layer_num].list.begin() + i);
            }
        }

        for (int new_elem_count = 0; new_elem_count < layers[layer_num].new_elem_amount; new_elem_count++) {
            Point try_point;
            while (true) {
                float angle = fixAngle((rand() / (float)RAND_MAX - 0.5) * max_angle + layers[layer_num].last_shift.getAngle());
                try_point = Point(sin(angle), cos(angle)) * (generation_range + (rand() / (float)RAND_MAX - 0.5) * layers[layer_num].shift_threshold) /*+ center*/;

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
            float rand_angle = -layers[layer_num].last_shift.getAngle() + (rand() / (float)RAND_MAX - 0.5) * PI;
            Point speed = Point(sin(rand_angle), cos(rand_angle)) * layers[layer_num].max_speed * (rand() / (float)RAND_MAX);
            layers[layer_num].list.push_back(Element(try_point, layer_num, speed));
        }

        for (int i = 0; i < layers[layer_num].list.size(); i++) {
            if ((layers[layer_num].list[i].pos).getLength() > delete_range) {
                layers[layer_num].list.erase(layers[layer_num].list.begin() + i);
            }
        }

        layers[layer_num].last_shift = Point();
	}

	void moveBackground(Point shift) {
		for (int layer = 0; layer < layers.size(); layer++) {
			for (int i = 0; i < layers[layer].list.size(); i++) {
				layers[layer].list[i].pos -= shift * layers[layer].move_coef - layers[layer].list[i].speed;
			}
		}
	}

	void uploadTextures(std::string path) {
		if (settings.isSpriteDebugOutputEnabled()) {
			std::cout << " -- Background sprites loading start -- " << std::endl;
		}

		std::vector<std::string> * object_names = getFileList(path);

		texture_buffer.resize(background_elem_names.size());
		for (int obj_cnt = 0; obj_cnt < object_names->size(); obj_cnt++) {
            if (settings.isSpriteDebugOutputEnabled()) {
                std::cout << " --- Background sprite " << (path + "\\" + (*object_names)[obj_cnt]) << " loading -- " << std::endl;
            }
			std::vector<std::string> * textures_set = getFileList(path + "\\" + (*object_names)[obj_cnt]);
			std::vector<sf::Texture> animation(textures_set->size());

            if (settings.isSpriteDebugOutputEnabled()) {
                std::cout << " --- sprite count " << textures_set->size() << " --- " << std::endl;
            }

			for (int i = 0; i < textures_set->size(); i++) {
				std::string sprite_path = (path + "\\" + ((*object_names)[obj_cnt] + "\\" + (*textures_set)[i])).c_str();
                if (settings.isSpriteDebugOutputEnabled()) {
                    std::cout << " ---- sprite " << (path + "\\" + ((*object_names)[obj_cnt] + "\\" + (*textures_set)[i])) << " ---- " << std::endl;
                }
				animation[i].loadFromFile(sprite_path);
			}

			if (is_background_name((*object_names)[obj_cnt]) != -1) {
                if (settings.isSpriteDebugOutputEnabled()) {
                    std::cout << " --- animation attach for " << (*object_names)[obj_cnt] << " to number " << is_background_name((*object_names)[obj_cnt]) << " --- " << std::endl;
                }
				texture_buffer[is_background_name((*object_names)[obj_cnt])] = animation;
            }
            else {
                if (settings.isErrorOutputEnabled()) {
                    std::cout << "Wrond background element name" << std::endl;
                }
            }
		}
		if (settings.isSpriteDebugOutputEnabled()) {
			std::cout << " -- Background sprites loading completed -- " << std::endl << std::endl;

            std::cout << " - Summary:" << std::endl;
            std::cout << " - Sprites amount: " << texture_buffer.size() << std::endl;
            for (int i = 0; i < texture_buffer.size(); i++) {
                std::cout << " -- " << texture_buffer[i].size() << std::endl;
            }
            std::cout << " - Summary end" << std::endl;
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

    void backgroundInfoParser(std::string path) {
        std::ifstream back_info;
        back_info.open(path);

        std::string input;
        back_info >> input;

        while (input.size() && !back_info.fail()) {
            if (input != "back_elem_start" || back_info.fail()) {
                if (settings.isErrorOutputEnabled()) {
                    std::cout << "Background element broken >> \""  << "\"" << std::endl;
                }
                back_info.close();
                return;
            }

            background_elem_names.resize(background_elem_names.size() + 1);
            layers.resize(background_elem_names.size());

            back_info >> input;
            if (input != "elem_name" || back_info.fail()) {
                if (settings.isErrorOutputEnabled()) {
                    std::cout << "Background element name broken" << std::endl;
                }
                back_info.close();
                return;
            }
            else {
                back_info >> input;
                background_elem_names[background_elem_names.size() - 1] = input;
            }

            back_info >> input;
            if (input != "move_coef") {
                if (settings.isErrorOutputEnabled()) {
                    std::cout << "Background element move coef broken" << std::endl;
                }
                back_info.close();
                return;
            }
            else {
                back_info >> input;
                layers[layers.size() - 1].move_coef = stof(input);
            }

            back_info >> input;
            if (input != "min_range") {
                if (settings.isErrorOutputEnabled()) {
                    std::cout << "Background element minimal range broken" << std::endl;
                }
                back_info.close();
                return;
            }
            else {
                back_info >> input;
                layers[layers.size() - 1].min_range = stof(input);
            }

            back_info >> input;
            if (input != "new_elem_amount") {
                if (settings.isErrorOutputEnabled()) {
                    std::cout << "Background element new elem amount broken" << std::endl;
                }
                back_info.close();
                return;
            }
            else {
                back_info >> input;
                layers[layers.size() - 1].new_elem_amount = stof(input);
            }

            back_info >> input;
            if (input != "max_speed") {
                if (settings.isErrorOutputEnabled()) {
                    std::cout << "Background element max speed broken" << std::endl;
                }
                back_info.close();
                return;
            }
            else {
                back_info >> input;
                layers[layers.size() - 1].max_speed = stof(input);
            }

            back_info >> input;
            if (input != "shift_threshold") {
                if (settings.isErrorOutputEnabled()) {
                    std::cout << "Background element shift threshold broken" << std::endl;
                }
                back_info.close();
                return;
            }
            else {
                back_info >> input;
                layers[layers.size() - 1].shift_threshold = stof(input);
            }

            back_info >> input;
            if (input != "back_elem_end") {
                if (settings.isErrorOutputEnabled()) {
                    std::cout << "Background element broken(end)" << std::endl;
                }
                back_info.close();
                return;
            }


            back_info >> input; // to check next element
        }
    }

public:

	BackgroundManager() {
        backgroundInfoParser("background_config.cfg");

		uploadTextures("background_components");
		initSprites();
	}

	void processFrame(Point pos_shift, Point center) {
		moveBackground(pos_shift);
        for (int layer = 0; layer < layers.size(); layer++) {
            layers[layer].last_shift += pos_shift * layers[layer].move_coef;
            if (layers[layer].last_shift.getLength() > layers[layer].shift_threshold) {
                refreshMap(layer);
            }
        }
	}

	void draw(sf::RenderWindow & window, Point center) {
		for (int layer = 0; layer < layers.size(); layer++) {
            for (int i = 0; i < layers[layer].list.size(); i++) {
                Point pos = layers[layer].list[i].pos + center;
                sprite_buffer[layer].setTexture(texture_buffer[layer][(layers[layer].list[i].frame_shift / 24) % texture_buffer[layer].size()]);
                sprite_buffer[layer].setPosition(pos.x * window.getSize().x / 1920, pos.y * window.getSize().y / 1080);

                layers[layer].list[i].frame_shift++;
                window.draw(sprite_buffer[layer]);
            }
		}
	}

    void generateAroundCenter(Point center) {
        for (int layer = 0; layer < layers.size(); layer++) {
			layers[layer].list.clear();
			for (float pos_x = -generation_range * 2; pos_x < 0; pos_x += layers[layer].shift_threshold) {
				layers[layer].last_shift = Point(1, 0);
				for (int i = 0; i < layers[layer].list.size(); i++) {
					layers[layer].list[i].pos -= Point(layers[layer].shift_threshold, 0);
				}
				refreshMap(layer);
			}
        }
    }

} background_manager;