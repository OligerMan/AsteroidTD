#pragma once

#include <vector>
#include <string>

#include "Object.h"
#include "EventBuffer.h"
#include "GUIParser.h"


enum textboxList {
	top_sign,
	gold_sign,
	research_sign,
	skill_status_sign,
	rank_start,
	rank_end = rank_start + 11,  // reserved for online rank

	SIGN_NUM
};

class GUIManager {

	std::vector<std::string> start_game_signs{
		std::string("Let's start"),
		std::string("GL HF!"),
		std::string("Show your skill"),
		std::string("Let's see how far you can go"),
		std::string("New stranger on the battlefield!"),
		std::string("What a new ship I see in outer space?"),
		std::string("Welcome back"),
		std::string("Hello, Mr(s).Stranger. Nice to meet you again"),
		std::string("Welcome to the battlefield"),
		std::string("Happy Hunger Games! And may the odds be ever in your favor"),
		std::string("Hello there\n<Obi-Wan Kenobi.jpg>")
	};

	std::vector<std::vector<Object *>> gui_elements;

	EventBuffer gui_event_buffer;
	EventBuffer external_event_buffer;

	Object * last_clicked_object = nullptr;
	Object * selected_object = nullptr;

	sf::Font base_font;
	std::vector<std::pair<sf::Text *, int>> text;

	bool checkClick(Point click) {
		Object * cursor = new Object(click);

		if (!gui_elements.empty()) {
			for (int layer = (int)gui_elements.size() - 1; layer >= 0; layer--) {
				for (int i = 0; i < gui_elements[layer].size(); i++) {
					
					Point pos = gui_elements[layer][i]->getPosition();
					if (layer == 1 && (pos - click).getLength() <= 50) {
						if (gui_elements[layer][i] != last_clicked_object) {

							gui_event_buffer.addEvent(clicked, gui_elements[layer][i], nullptr);
							//std::cout << "Click" << std::endl;

							last_clicked_object = gui_elements[layer][i];
						}
						else {
							gui_event_buffer.addEvent(pressed, gui_elements[layer][i], nullptr);
							//std::cout << "Press" << std::endl;
						}
						delete cursor;
						return true;
					}
					else if (last_clicked_object == gui_elements[layer][i]) {
						gui_event_buffer.addEvent(released, gui_elements[layer][i], nullptr);
						//std::cout << "Release" << std::endl;

						last_clicked_object = nullptr;
					}
				}
			}
		}
		delete cursor;
		return false;
	}

	void processEventBuffer() {
		Event cur_event = gui_event_buffer.getEvent();
		if (cur_event.getEventType() != null_event) {
			if (cur_event.getEventType() == clicked && static_cast<Object *>(cur_event.getData(0))->getObjectType() != redactor_back) {
				if (selected_object) {
					delete selected_object;
				}
				selected_object = new Object(*static_cast<Object *>(cur_event.getData(0)));
				selected_object->setAutoOrigin();
			}
		}
	}

public:

	GUIManager() {
		base_font.loadFromFile(consts.getDefaultFontPath());
		srand(time(NULL));
		text.resize(1, std::pair<sf::Text *, int>(new sf::Text, 0));
		text[0].first->setString(start_game_signs[rand() % (start_game_signs.size())]);
		text[0].first->setFillColor(sf::Color::White);
		text[0].first->setOutlineColor(sf::Color::Black);
		text[0].first->setOutlineThickness(1);
		text[0].first->setCharacterSize(35);
		text[0].first->setFont(base_font);
		text[0].second = consts.getFPSLock() * 5;
	}

	bool loadFromFile(std::string path) {
		gui_parser(path, &gui_elements);
	}

	bool processFrame(Point click, Point viewport_pos) {
		bool output = checkClick(click);
		processEventBuffer();

		return output;
	}

	Event getEvent() {
		return external_event_buffer.getEvent();
	}

	std::vector<std::vector<Object *>> * getObjectsBuffer() {
		return &gui_elements;
	}

	Object * getSelectedObject() {
		return selected_object;
	}

	std::vector<std::pair<sf::Text *, int>> & getGUIText() {
		return text;
	}

	void setTopSign(std::string string, float time /*in seconds*/) {
		text[0].first->setString(string);
		text[0].second = consts.getFPSLock() * time;
	}

    void setTopSign(std::wstring string, float time /*in seconds*/) {
        text[0].first->setString(string);
        text[0].second = consts.getFPSLock() * time;
    }

	void setTopSign(char * string, float time /*in seconds*/) {
		if (text[0].second <= 0) {
			text[0].first->setString(string);
			text[0].second = consts.getFPSLock() * time;
		}
	}

	void forceSetTopSign(std::string string, float time /*in seconds*/) {
		text[0].first->setString(string);
		text[0].second = consts.getFPSLock() * time;
	}

    void forceSetTopSign(std::wstring string, float time /*in seconds*/) {
        text[0].first->setString(string);
        text[0].second = consts.getFPSLock() * time;
    }

	void setText(std::string string, float time, int number, Point pos, int character_size) {
		if (number < 0 || time < 0 || character_size < 0) {
			return;
		}
		if (text.size() <= number) {
			text.resize(number + 1, std::pair<sf::Text *, int>(new sf::Text, 0));
		}
		text[number].first->setString(string);
		text[number].first->setPosition(sf::Vector2f(pos.x, pos.y));
		text[number].first->setFillColor(sf::Color::White);
		text[number].first->setOutlineColor(sf::Color::Black);
		text[number].first->setOutlineThickness(1);
		text[number].first->setCharacterSize(character_size);
		text[number].first->setFont(base_font);
		text[number].second = consts.getFPSLock() * time;
	}

    void setText(std::wstring string, float time, int number, Point pos, int character_size) {
        if (number < 0 || time < 0 || character_size < 0) {
            return;
        }
        if (text.size() <= number) {
            text.resize(number + 1, std::pair<sf::Text *, int>(new sf::Text, 0));
        }
        text[number].first->setString(string);
        text[number].first->setPosition(sf::Vector2f(pos.x, pos.y));
        text[number].first->setFillColor(sf::Color::White);
        text[number].first->setOutlineColor(sf::Color::Black);
        text[number].first->setOutlineThickness(1);
        text[number].first->setCharacterSize(character_size);
        text[number].first->setFont(base_font);
        text[number].second = consts.getFPSLock() * time;
    }
};