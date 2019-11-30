#pragma once

#include <vector>

#include <SFML\Graphics.hpp>

#include "FileSearch.h"
#include "GameStatus.h"
#include "ResourceManager.h"


class GUIVisualController{
	std::vector<std::vector<std::vector<sf::Texture>>> texture_buffer;
	std::vector<std::vector<sf::Sprite>> sprite_buffer;

	std::vector<std::vector<sf::Texture>> skills_texture;
	std::vector<std::vector<sf::Sprite>> skills_sprite;

	bool is_active = true;

	enum {
		ready,
		not_enough_money,

		SKILL_STATE_AMOUNT
	};

	enum {
		dome_struct,
		gold_struct,
		science_struct,
		turret_struct,
		rocket_skill,
		attack_buff_skill,
		heal_skill,
		speed_boost_skill,

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

		skills_texture[dome_struct][ready].loadFromFile("skill_sprite\\dome\\ready.png");
		skills_texture[dome_struct][not_enough_money].loadFromFile("skill_sprite\\dome\\notready.png");
		skills_texture[turret_struct][ready].loadFromFile("skill_sprite\\turret\\ready.png");
		skills_texture[turret_struct][not_enough_money].loadFromFile("skill_sprite\\turret\\notready.png");
		skills_texture[gold_struct][ready].loadFromFile("skill_sprite\\gold\\ready.png");
		skills_texture[gold_struct][not_enough_money].loadFromFile("skill_sprite\\gold\\notready.png");
		skills_texture[science_struct][ready].loadFromFile("skill_sprite\\science\\ready.png");
		skills_texture[science_struct][not_enough_money].loadFromFile("skill_sprite\\science\\notready.png");
		skills_texture[rocket_skill][ready].loadFromFile("skill_sprite\\rocket\\ready.png");
		skills_texture[rocket_skill][not_enough_money].loadFromFile("skill_sprite\\rocket\\notready.png");
		skills_texture[speed_boost_skill][ready].loadFromFile("skill_sprite\\speed\\ready.png");
		skills_texture[speed_boost_skill][not_enough_money].loadFromFile("skill_sprite\\speed\\notready.png");
		skills_texture[attack_buff_skill][ready].loadFromFile("skill_sprite\\attack\\ready.png");
		skills_texture[attack_buff_skill][not_enough_money].loadFromFile("skill_sprite\\attack\\notready.png");
		skills_texture[heal_skill][ready].loadFromFile("skill_sprite\\heal\\ready.png");
		skills_texture[heal_skill][not_enough_money].loadFromFile("skill_sprite\\heal\\notready.png");

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
			//Point center = Point(window->getSize().x / 2 - consts.getSkillsIconBorder() - (consts.getSkillsIconShiftRadius() + consts.getSkillsIconSize() / 2) * window->getView().getSize().x / 1920, window->getSize().y / 2 - consts.getSkillsIconBorder() - (consts.getSkillsIconShiftRadius() + consts.getSkillsIconSize() / 2) * window->getView().getSize().y / 1080);
			Point center = Point(window->getView().getSize().x / 2 - (consts.getSkillsIconSize() + consts.getSkillsIconShiftRadius() + consts.getSkillsIconBorder()) * window->getView().getSize().x / 1920, window->getView().getSize().y / 2 - (consts.getSkillsIconSize() + consts.getSkillsIconShiftRadius() + consts.getSkillsIconBorder()) * window->getView().getSize().y / 1080);
			for (int j = 0; j < skills_texture[0].size(); j++) {
				skills_sprite[i][j].setTexture(skills_texture[i][j]);
				sf::Vector2u v = skills_texture[i][j].getSize();
				float coef = consts.getSkillsIconSize();
				if (v.x != 0 && v.y != 0) {
					skills_sprite[i][j].setScale(coef / v.x, coef / v.y);
				}
				Point pos = center + Point(cos((float)(i % 4) / 4.0 * PI * 2 - PI / 2), sin((float)(i % 4) / 4.0 * PI * 2 - PI / 2)) * consts.getSkillsIconShiftRadius() * window->getView().getSize().y / 1080;
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
		const std::string texture_path = "GUI";

		uploadTextures(texture_path);
		initSprites();
	}

	void setActive() {
		is_active = true;
	}

	bool isActive() {
		return is_active;
	}

	bool processFrame(sf::RenderWindow * window, std::vector<std::vector<Object *>> * objects, std::vector<std::pair<sf::Text *, int>> & text, Point viewport_pos, float hero_hp_percent) {  // render for GUI elements
		if (game_status == pause || game_status == game_strategic_mode) {
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
				
				window->draw(*sprite);
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
				skills_sprite[i][j].setScale(sf::Vector2f(consts.getSkillsIconSize() / skills_texture[i][j].getSize().x * window->getView().getSize().x / 1920, consts.getSkillsIconSize() / skills_texture[i][j].getSize().y * window->getView().getSize().y / 1080));

				sf::Vector2f v = skills_sprite[i][j].getScale();
				//skills_sprite[i][j].setOrigin(-viewport_pos.x * v.x, -viewport_pos.y * v.y);
				skills_sprite[i][j].setOrigin(-window->getView().getCenter().x / v.x, -window->getView().getCenter().y / v.y);
			}
		}

        if (game_status == GameStatus::game_hero_mode) {

            if (skills_mode == SkillsMode::set1) {
                if (resource_manager.isEnoughGold(consts.getDamageBuffPrice())) {
                    window->draw(skills_sprite[attack_buff_skill][ready]);
                }
                else {
                    window->draw(skills_sprite[attack_buff_skill][not_enough_money]);
                }
                if (resource_manager.isEnoughGold(consts.getSpeedBuffPrice())) {
                    window->draw(skills_sprite[speed_boost_skill][ready]);
                }
                else {
                    window->draw(skills_sprite[speed_boost_skill][not_enough_money]);
                }
                if (resource_manager.isEnoughGold(consts.getAttackAbilityPrice())) {
                    window->draw(skills_sprite[rocket_skill][ready]);
                }
                else {
                    window->draw(skills_sprite[rocket_skill][not_enough_money]);
                }
                if (resource_manager.isEnoughGold(consts.getHealBuffPrice())) {
                    window->draw(skills_sprite[heal_skill][ready]);
                }
                else {
                    window->draw(skills_sprite[heal_skill][not_enough_money]);
                }
            }
            else {
                if (resource_manager.isEnoughGold(consts.getBaseDomePrice())) {
                    window->draw(skills_sprite[dome_struct][ready]);
                }
                else {
                    window->draw(skills_sprite[dome_struct][not_enough_money]);
                }
                if (resource_manager.isEnoughGold(consts.getBaseSciencePrice())) {
                    window->draw(skills_sprite[science_struct][ready]);
                }
                else {
                    window->draw(skills_sprite[science_struct][not_enough_money]);
                }
                if (resource_manager.isEnoughGold(consts.getBaseGoldPrice())) {
                    window->draw(skills_sprite[gold_struct][ready]);
                }
                else {
                    window->draw(skills_sprite[gold_struct][not_enough_money]);
                }
                if (resource_manager.isEnoughGold(consts.getBaseTurretPrice())) {
                    window->draw(skills_sprite[turret_struct][ready]);
                }
                else {
                    window->draw(skills_sprite[turret_struct][not_enough_money]);
                }
            }
        }

		// hero HP draw
		sf::RectangleShape hp_rectangle(sf::Vector2f(window->getView().getSize().x * hero_hp_percent, window->getView().getSize().y / 40));
		hp_rectangle.setPosition(sf::Vector2f(-(int)window->getView().getSize().x / 2, window->getView().getSize().y * 19 / 40));
		hp_rectangle.setOrigin(-viewport_pos.x, -viewport_pos.y);
		hp_rectangle.setFillColor(sf::Color(0,122,204));
		window->draw(hp_rectangle);

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