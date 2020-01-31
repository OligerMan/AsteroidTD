#pragma once

#include <vector>
#include <chrono>
#include <iostream>

#include <SFML\Graphics.hpp>

#include "GeometryComponents.h"


struct ButtonList {
	std::vector<Point> button_list;
	int cur_index = 0;
	std::chrono::time_point<std::chrono::steady_clock> last_choice = std::chrono::steady_clock::now();
	int delay = 1000;   // in milliseconds
	float angle = PI / 4;
};

class ButtonSelector {
	std::vector<ButtonList> buffer;

public:

	enum {
		main_menu,
		research,
		game_over,
		mission_list,
		completed_mission_list,
	};

	void processButtonBuffer(int buf_num) {
		bool is_input_state = false;
		bool is_keyboard_input = false;
		Point move_vector;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				move_vector += Point(0, -1);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				move_vector += Point(-1, 0);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				move_vector += Point(0, 1);
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				move_vector += Point(1, 0);
			}
			is_keyboard_input = true;
		}
		else if (sf::Joystick::isConnected(0)) {         // gamepad input

			move_vector = Point(
				sf::Joystick::getAxisPosition(0, sf::Joystick::X),
				sf::Joystick::getAxisPosition(0, sf::Joystick::Y));

			if (abs(move_vector.x) < 5 && abs(move_vector.y) < 5) {
				move_vector = Point();
			}
		}
		move_vector.normalize();

		if (move_vector.x != 0 || move_vector.y != 0) {

			is_input_state = true;

			int next_index = -1;
			double min_distance = 1500;

			for (int i = 0; i < buffer[buf_num].button_list.size(); i++) {
				double angle_diff =
					(std::atan2(move_vector.x, move_vector.y) -
						std::atan2(
							buffer[buf_num].button_list[i].x - buffer[buf_num].button_list[buffer[buf_num].cur_index].x,
							buffer[buf_num].button_list[i].y - buffer[buf_num].button_list[buffer[buf_num].cur_index].y));

				if (abs(angle_diff) > 0.000001) {     // angle_diff is not close to zero
					if (abs(angle_diff) > abs(angle_diff + 2 * PI)) {
						angle_diff += 2 * PI;
					}
					if (abs(angle_diff) > abs(angle_diff - 2 * PI)) {
						angle_diff -= 2 * PI;
					}
				}
				if (abs(angle_diff) <= buffer[buf_num].angle / (is_keyboard_input ? 1 : 2)) {
					if ((next_index == -1 || ((buffer[buf_num].button_list[i] - buffer[buf_num].button_list[buffer[buf_num].cur_index]).getLength() < (buffer[buf_num].button_list[next_index] - buffer[buf_num].button_list[buffer[buf_num].cur_index]).getLength())) && (min_distance > (buffer[buf_num].button_list[i] - buffer[buf_num].button_list[buffer[buf_num].cur_index]).getLength()) && (buffer[buf_num].button_list[i] - buffer[buf_num].button_list[buffer[buf_num].cur_index]).getLength() > 0.01 /*not close to zero*/) {
						next_index = i;
						min_distance = (buffer[buf_num].button_list[i] - buffer[buf_num].button_list[buffer[buf_num].cur_index]).getLength();
					}
				}
			}

			int milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - buffer[buf_num].last_choice).count();
			if (next_index != -1 && milliseconds > buffer[buf_num].delay) {
				buffer[buf_num].cur_index = next_index;
				buffer[buf_num].last_choice = std::chrono::steady_clock::now();
			}
		}
		if (!is_input_state) {
			buffer[buf_num].last_choice = std::chrono::steady_clock::now() - std::chrono::seconds(500);
		}
	}

	ButtonList getButtonList(int buf_num) {
		if (buf_num < 0 || buf_num >= buffer.size()) {
			return ButtonList();
		}
		return buffer[buf_num];
	}

	void initButtonList(int buf_num, int new_delay, float new_angle, std::vector<Point> positions) {
		if (buf_num < 0) {
			return;
		}
		if (buf_num >= buffer.size()) {
			buffer.resize(buf_num + 1);
		}

		buffer[buf_num].button_list = positions;
		buffer[buf_num].cur_index = 0;
		buffer[buf_num].delay = new_delay;
		buffer[buf_num].angle = new_angle;
		buffer[buf_num].last_choice = std::chrono::steady_clock::now();
	}

	void resetButtonList(int buf_num, std::vector<Point> positions) {
		if (buf_num < 0) {
			return;
		}
		if (buf_num >= buffer.size()) {
			buffer.resize(buf_num + 1);
		}
		buffer[buf_num].button_list = positions;
		if (buffer[buf_num].cur_index >= buffer.size()) {
			if (buffer[buf_num].button_list.size()) {
				buffer[buf_num].cur_index = buffer[buf_num].button_list.size() - 1;
			}
		}
	}

	void deleteButton(int buf_num, int button_num) {
		buffer[buf_num].button_list.erase(buffer[buf_num].button_list.begin() + button_num);
		if (buffer[buf_num].cur_index == buffer[buf_num].button_list.size()) {
			if (buffer[buf_num].button_list.size()) {
				buffer[buf_num].cur_index = buffer[buf_num].button_list.size() - 1;
			}
		}
	}
} button_selector;