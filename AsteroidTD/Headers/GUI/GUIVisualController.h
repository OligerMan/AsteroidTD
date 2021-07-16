#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "../Utils/FileSearch.h"
#include "../Game/GameStatus.h"
#include "../Game/ResourceManager.h"
#include "../../ResourcePath.hpp"


class GUIVisualController{
	std::vector<std::vector<std::vector<sf::Texture>>> texture_buffer;
	std::vector<std::vector<sf::Sprite>> sprite_buffer;

	std::vector<std::vector<sf::Texture>> skills_texture;
	std::vector<std::vector<sf::Sprite>> skills_sprite;

	bool is_active = true;

    sf::Font base_font;
    sf::Text skill_price_sign;

	enum {
		ready,
		not_enough_money,

		SKILL_STATE_AMOUNT
	};

	enum {
		dome_struct,
		gold_struct,
        turret_struct,
		science_struct,
		rocket_skill,
        speed_boost_skill,
		attack_buff_skill,
		heal_skill,

		dialog_start,

		SKILL_AMOUNT
	};

	void uploadTextures(std::string path) {
		if (settings.isSpriteDebugOutputEnabled()) {
			std::cout << " -- GUI sprites loading start -- " << std::endl;
		}

		// main sprites loading


		std::vector<std::string> * object_names = getFileList(path);

		texture_buffer.resize((int)COUNT_OBJECT);
		for (int obj_cnt = 0; obj_cnt < object_names->size(); obj_cnt++) {
			if (settings.isSpriteDebugOutputEnabled()) {
				std::cout << "GUI object " << (*object_names)[obj_cnt] << " sprite loading" << std::endl;
			}

			//get animation set
			std::vector<std::string> * anim_set = getFileList(path + path_separator + (*object_names)[obj_cnt]);
			std::vector<std::vector<sf::Texture>> object_textures;

			texture_buffer[is_object_type_exists((*object_names)[obj_cnt])].resize((int)COUNT_ANIMATION);
			for (int anim_cnt = 0; anim_cnt < anim_set->size(); anim_cnt++) {
				if (settings.isSpriteDebugOutputEnabled()) {
					std::cout << "    >> Animation " << (*anim_set)[anim_cnt] << " is loading" << std::endl;
				}

				//get sprites set
				std::vector<std::string> * textures_set = getFileList(path + path_separator + ((*object_names)[obj_cnt] + path_separator + (*anim_set)[anim_cnt]));
				std::vector<sf::Texture> animation(textures_set->size());

				for (int i = 0; i < textures_set->size(); i++) {
					if (settings.isSpriteDebugOutputEnabled()) {
						std::cout << "        >> Sprite " << (*textures_set)[i] << " is loading" << std::endl;
					}
					std::string sprite_path = (path + path_separator + ((*object_names)[obj_cnt] + path_separator + (*anim_set)[anim_cnt] + path_separator + (*textures_set)[i])).c_str();
					animation[i].loadFromFile(sprite_path);
				}
				if (is_animation_type_exists((*anim_set)[anim_cnt]) != -1 && is_object_type_exists((*object_names)[obj_cnt]) != -1) {
					texture_buffer[is_object_type_exists((*object_names)[obj_cnt])][is_animation_type_exists((*anim_set)[anim_cnt])] = animation;
				}
				delete textures_set;
			}

			delete anim_set;
		}
		delete object_names;

		// skills sprites loading


		skills_texture.resize(SKILL_AMOUNT);
		skills_sprite.resize(SKILL_AMOUNT);
		for (int i = 0; i < SKILL_AMOUNT; i++) {
			skills_texture[i].resize(SKILL_STATE_AMOUNT);
			skills_sprite[i].resize(SKILL_STATE_AMOUNT);
		}

		skills_texture[dome_struct][ready].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "dome" + path_separator + "ready.png");
		skills_texture[dome_struct][not_enough_money].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "dome" + path_separator + "notready.png");
		skills_texture[turret_struct][ready].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "turret" + path_separator + "ready.png");
		skills_texture[turret_struct][not_enough_money].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "turret" + path_separator + "notready.png");
		skills_texture[gold_struct][ready].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "gold" + path_separator + "ready.png");
		skills_texture[gold_struct][not_enough_money].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "gold" + path_separator + "notready.png");
		skills_texture[science_struct][ready].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "science" + path_separator + "ready.png");
		skills_texture[science_struct][not_enough_money].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "science" + path_separator + "notready.png");
		skills_texture[rocket_skill][ready].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "rocket" + path_separator + "ready.png");
		skills_texture[rocket_skill][not_enough_money].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "rocket" + path_separator + "notready.png");
		skills_texture[speed_boost_skill][ready].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "speed" + path_separator + "ready.png");
		skills_texture[speed_boost_skill][not_enough_money].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "speed" + path_separator + "notready.png");
		skills_texture[attack_buff_skill][ready].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "attack" + path_separator + "ready.png");
		skills_texture[attack_buff_skill][not_enough_money].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "attack" + path_separator + "notready.png");
		skills_texture[heal_skill][ready].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "heal" + path_separator + "ready.png");
		skills_texture[heal_skill][not_enough_money].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "heal" + path_separator + "notready.png");
		skills_texture[dialog_start][ready].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "dialog_start" + path_separator + "ready.png");
		skills_texture[dialog_start][not_enough_money].loadFromFile(resourcePath() + "Skill_sprite" + path_separator + "dialog_start" + path_separator + "ready.png");

		for (int i = 0; i < SKILL_AMOUNT; i++) {
			for (int j = 0; j < SKILL_STATE_AMOUNT; j++) {
				skills_sprite[i][j].setTexture(skills_texture[i][j]);
				sf::Vector2u v = skills_texture[i][j].getSize();
				float coef = consts.getSkillsIconSize();
				if (v.x != 0 && v.y != 0) {
					skills_sprite[i][j].setScale(coef / v.x, coef / v.y);
				}
			}
		}

		if (settings.isSpriteDebugOutputEnabled()) {
			std::cout << " -- GUI sprites loading completed -- " << std::endl << std::endl;
		}

		return;
	}

	void setSkillsIconPosition(sf::RenderWindow * window) {
		for (int i = 0; i < skills_texture.size(); i++) {
			Point center = Point(window->getView().getSize().x / 2 - (4 * consts.getSkillsIconSize() + consts.getSkillsIconBorder() / 2) * window->getView().getSize().x / 1920, window->getView().getSize().y / 2 - (consts.getSkillsIconSize() + consts.getSkillsIconBorder()) * window->getView().getSize().y / 1080);
			for (int j = 0; j < skills_texture[0].size(); j++) {
				skills_sprite[i][j].setTexture(skills_texture[i][j]);
				sf::Vector2u v = skills_texture[i][j].getSize();
				float coef = consts.getSkillsIconSize();
				
				if (v.x != 0 && v.y != 0) {
					skills_sprite[i][j].setScale(coef / v.x, coef / v.y);
				}
                //Point pos = center + Point(cos((float)(i % 4) / 4.0 * PI * 2 - PI / 2), sin((float)(i % 4) / 4.0 * PI * 2 - PI / 2)) * consts.getSkillsIconShiftRadius() * window->getView().getSize().y / 1080;
                Point pos = center + Point(i % 4, 0) * consts.getSkillsIconShiftRadius() * window->getView().getSize().y / 1080;
				if (i == dialog_start) {
					pos = center - Point(1, 1) * consts.getSkillsIconSize();
				}
				skills_sprite[i][j].setPosition(sf::Vector2f(pos.x, pos.y));
			}
		}
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

	GUIVisualController() {
		const std::string texture_path = resourcePath() + "GUI";

		uploadTextures(texture_path);
		initSprites();

        base_font.loadFromFile(consts.getDefaultFontPath());

        skill_price_sign.setFillColor(sf::Color::White);
        skill_price_sign.setOutlineColor(sf::Color::Black);
        skill_price_sign.setOutlineThickness(1);
        skill_price_sign.setFont(base_font);
	}

	void setActive() {
		is_active = true;
	}

	bool isActive() {
		return is_active;
	}

	bool processFrame(sf::RenderWindow * window, std::vector<std::vector<Object *>> * objects, std::vector<std::pair<sf::Text *, int>> & text, Point viewport_pos, float hero_hp_percent, float demolition_price) {  // render for GUI elements
		if (game_status == game_pause || game_status == game_strategic_mode) {
			hero_hp_percent = 0;
		}


		// main gui elements render
		for (int layer = 0; layer < objects->size(); layer++) {
			for (int i = 0; i < (*objects)[layer].size(); i++) {
				Object * object = (*objects)[layer][i];

				int obj_type = object->getObjectSpriteType();
				int anim_type = object->getObjectAnimationType();
				int frame_num = object->getFrameNumber();

				Point position = object->getPosition() + viewport_pos;
				double angle = object->getAngle();

				sf::Sprite * sprite = &(sprite_buffer[obj_type][anim_type]);

				if ((texture_buffer.size() > obj_type) && (texture_buffer[obj_type].size() > anim_type) && (texture_buffer[obj_type][anim_type].size() > frame_num)) {
					sprite->setTexture(texture_buffer[obj_type][anim_type][frame_num]);
				}
				else {
					sprite->setTexture(texture_buffer[0][0][0]);
				}
				sprite->setPosition(position.x, position.y);
				sprite->setOrigin(object->getOrigin().x, object->getOrigin().y);
				sprite->setRotation(angle);
				sprite->setScale(sf::Vector2f(window->getView().getSize().x / 1920, window->getView().getSize().y / 1080));
				

                if (!cinematic_mode) {
                    window->draw(*sprite);
                }
			}
		}

		// gui text render
		text[0].first->setPosition(sf::Vector2f(0, -window->getView().getSize().y / 2 / 1.2));
		for (int i = 0; i < text.size(); i++) {
			sf::Vector2f prev_pos = text[i].first->getPosition();
			int prev_font_size = text[i].first->getCharacterSize();
			text[i].first->setCharacterSize(text[i].first->getCharacterSize() * window->getView().getSize().y / 1080);
			if (i <= text.size()) {
				text[i].first->setOrigin(-viewport_pos.x, -viewport_pos.y);
			}
			if (i <= 0) {
				text[i].first->setOrigin(-viewport_pos.x + text[i].first->getLocalBounds().width / 2, -viewport_pos.y + text[i].first->getLocalBounds().height / 2);
			}
			if (text[i].second > 0) {
				window->draw(*text[i].first);
				text[i].second--;
			}
			text[i].first->setPosition(prev_pos);
			text[i].first->setCharacterSize(prev_font_size);
		}

		// skills icons render
		setSkillsIconPosition(window);
		for (int i = 0; i < SKILL_AMOUNT; i++) {
			for (int j = 0; j < SKILL_STATE_AMOUNT; j++) {
				float coef = (i == dialog_start) ? 2 : 1;
				skills_sprite[i][j].setScale(sf::Vector2f(consts.getSkillsIconSize() / skills_texture[i][j].getSize().x * window->getView().getSize().x / 1920 * coef, consts.getSkillsIconSize() / skills_texture[i][j].getSize().y * window->getView().getSize().y / 1080 * coef));

				sf::Vector2f v = skills_sprite[i][j].getScale();
				skills_sprite[i][j].setOrigin(-window->getView().getCenter().x / v.x, -window->getView().getCenter().y / v.y);
			}
		}

        //skill_price_sign.setPosition(pos.x, pos.y);
        skill_price_sign.setOrigin(
             - 
            window->getView().getCenter().x - 
            consts.getSkillsIconSize(), 
            skill_price_sign.getLocalBounds().height / 2 - 
            window->getView().getCenter().y - 
            consts.getSkillsIconSize());

        skill_price_sign.setCharacterSize(60 * window->getSize().y / 1080);

        if (game_status == GameStatus::game_hero_mode) {

            if (skills_mode == SkillsMode::set1) {
                if (resource_manager.isEnoughGold(consts.getDamageBuffPrice())) {

                    if (!cinematic_mode) {
                        window->draw(skills_sprite[attack_buff_skill][ready]);
                    }
                }
                else {

                    if (!cinematic_mode) {
                        window->draw(skills_sprite[attack_buff_skill][not_enough_money]);
                    }
                }
                skill_price_sign.setString(std::to_string((int)consts.getDamageBuffPrice()));
                skill_price_sign.setPosition(skills_sprite[attack_buff_skill][ready].getPosition() - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));

                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

                if (sf::Joystick::isConnected(0)) {
                    skill_price_sign.setString("B");
                }
                else {
                    skill_price_sign.setString("3");
                }
                skill_price_sign.setPosition(skills_sprite[attack_buff_skill][ready].getPosition() - sf::Vector2f(0, consts.getSkillsIconSize() + 15) - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));

                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

                if (resource_manager.isEnoughGold(consts.getSpeedBuffPrice())) {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[speed_boost_skill][ready]);
                    }
                }
                else {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[speed_boost_skill][not_enough_money]);
                    }
                }
                skill_price_sign.setString(std::to_string((int)consts.getSpeedBuffPrice()));
                skill_price_sign.setPosition(skills_sprite[speed_boost_skill][ready].getPosition() - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));
                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

                if (sf::Joystick::isConnected(0)) {
                    skill_price_sign.setString("X");
                }
                else {
                    skill_price_sign.setString("2");
                }
                skill_price_sign.setPosition(skills_sprite[speed_boost_skill][ready].getPosition() - sf::Vector2f(0, consts.getSkillsIconSize() + 15) - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));
                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

                if (resource_manager.isEnoughGold(consts.getAttackAbilityPrice())) {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[rocket_skill][ready]);
                    }
                }
                else {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[rocket_skill][not_enough_money]);
                    }
                }
                skill_price_sign.setString(std::to_string((int)consts.getAttackAbilityPrice()));
                skill_price_sign.setPosition(skills_sprite[rocket_skill][ready].getPosition() - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));

                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

                if (sf::Joystick::isConnected(0)) {
                    skill_price_sign.setString("Y");
                }
                else {
                    skill_price_sign.setString("1");
                }
                skill_price_sign.setPosition(skills_sprite[rocket_skill][ready].getPosition() - sf::Vector2f(0, consts.getSkillsIconSize() + 15) - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));

                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

				if (demolition_price > 0.0000001/*eps*/) {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[heal_skill][ready]);
                    }
                }
                else {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[heal_skill][not_enough_money]);
                    }
                }
                skill_price_sign.setString(std::to_string((int)demolition_price));
                skill_price_sign.setPosition(skills_sprite[heal_skill][ready].getPosition() - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));
                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

                if (sf::Joystick::isConnected(0)) {
                    skill_price_sign.setString("A");
                }
                else {
                    skill_price_sign.setString("4");
                }
                skill_price_sign.setPosition(skills_sprite[heal_skill][ready].getPosition() - sf::Vector2f(0, consts.getSkillsIconSize() + 15) - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));

                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }
            }
            else if (skills_mode == SkillsMode::set2) {
                if (resource_manager.isEnoughGold(consts.getBaseDomePrice())) {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[dome_struct][ready]);
                    }
                }
                else {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[dome_struct][not_enough_money]);
                    }
                }
                skill_price_sign.setString(std::to_string((int)consts.getBaseDomePrice()));
                skill_price_sign.setPosition(skills_sprite[dome_struct][ready].getPosition() - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));
                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

                if (sf::Joystick::isConnected(0)) {
                    skill_price_sign.setString("Y");
                }
                else {
                    skill_price_sign.setString("1");
                }
                skill_price_sign.setPosition(skills_sprite[dome_struct][ready].getPosition() - sf::Vector2f(0, consts.getSkillsIconSize() + 15) - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));

                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

                if (resource_manager.isEnoughGold(consts.getBaseSciencePrice())) {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[science_struct][ready]);
                    }
                }
                else {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[science_struct][not_enough_money]);
                    }
                }
                skill_price_sign.setString(std::to_string((int)consts.getBaseSciencePrice()));
                skill_price_sign.setPosition(skills_sprite[science_struct][ready].getPosition() - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));

                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

                if (sf::Joystick::isConnected(0)) {
                    skill_price_sign.setString("A");
                }
                else {
                    skill_price_sign.setString("4");
                }
                skill_price_sign.setPosition(skills_sprite[science_struct][ready].getPosition() - sf::Vector2f(0, consts.getSkillsIconSize() + 15) - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));

                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

                if (resource_manager.isEnoughGold(consts.getBaseGoldPrice())) {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[gold_struct][ready]);
                    }
                }
                else {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[gold_struct][not_enough_money]);
                    }
                }
                skill_price_sign.setString(std::to_string((int)consts.getBaseGoldPrice()));
                skill_price_sign.setPosition(skills_sprite[gold_struct][ready].getPosition() - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));

                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

                if (sf::Joystick::isConnected(0)) {
                    skill_price_sign.setString("X");
                }
                else {
                    skill_price_sign.setString("2");
                }
                skill_price_sign.setPosition(skills_sprite[gold_struct][ready].getPosition() - sf::Vector2f(0, consts.getSkillsIconSize() + 15) - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));

                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

                if (resource_manager.isEnoughGold(consts.getBaseTurretPrice())) {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[turret_struct][ready]);
                    }
                }
                else {
                    if (!cinematic_mode) {
                        window->draw(skills_sprite[turret_struct][not_enough_money]);
                    }
                }
                skill_price_sign.setString(std::to_string((int)consts.getBaseTurretPrice()));
                skill_price_sign.setPosition(skills_sprite[turret_struct][ready].getPosition() - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));

                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }

                if (sf::Joystick::isConnected(0)) {
                    skill_price_sign.setString("B");
                }
                else {
                    skill_price_sign.setString("3");
                }
                skill_price_sign.setPosition(skills_sprite[turret_struct][ready].getPosition() - sf::Vector2f(0, consts.getSkillsIconSize() + 15) - sf::Vector2f(skill_price_sign.getLocalBounds().width / 2, skill_price_sign.getLocalBounds().height / 2));

                if (!cinematic_mode) {
                    window->draw(skill_price_sign);
                }
			}
			else {

                if (!cinematic_mode) {
                    window->draw(skills_sprite[dialog_start][ready]);
                }
			}
        }

		// hero HP draw
		sf::RectangleShape hp_rectangle(sf::Vector2f(window->getView().getSize().x * hero_hp_percent, window->getView().getSize().y / 40));
		hp_rectangle.setPosition(sf::Vector2f(-(int)window->getView().getSize().x / 2, window->getView().getSize().y * 19 / 40));
		hp_rectangle.setOrigin(-viewport_pos.x, -viewport_pos.y);
		hp_rectangle.setFillColor(sf::Color(0,122,204));
        if (!cinematic_mode) {
            window->draw(hp_rectangle);
        }

		// skill icons draw
		
		return is_active;
	}

	void reset() {
		texture_buffer.clear();
		sprite_buffer.clear();
		skills_texture.clear();
		skills_sprite.clear();

		const std::string texture_path = "GUI";

		uploadTextures(texture_path);
		initSprites();
	}
};
