#pragma once

#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <SFML\Graphics.hpp>
#include <Windows.h>

class KeyboardBuffer {
	std::string buffer;

	enum KeyStat {
		idle,
		first_pressed,
		pressed,
	};
	std::vector<sf::Keyboard::Key> keys = {
		sf::Keyboard::Q,
		sf::Keyboard::W,
		sf::Keyboard::E,
		sf::Keyboard::R,
		sf::Keyboard::T,
		sf::Keyboard::Y,
		sf::Keyboard::U,
		sf::Keyboard::I,
		sf::Keyboard::O,
		sf::Keyboard::P,
		sf::Keyboard::A,
		sf::Keyboard::S,
		sf::Keyboard::D,
		sf::Keyboard::F,
		sf::Keyboard::G,
		sf::Keyboard::H,
		sf::Keyboard::J,
		sf::Keyboard::K,
		sf::Keyboard::L,
		sf::Keyboard::Z,
		sf::Keyboard::X,
		sf::Keyboard::C,
		sf::Keyboard::V,
		sf::Keyboard::B,
		sf::Keyboard::N,
		sf::Keyboard::M,
		sf::Keyboard::Num1,
		sf::Keyboard::Num2,
		sf::Keyboard::Num3,
		sf::Keyboard::Num4,
		sf::Keyboard::Num5,
		sf::Keyboard::Num6,
		sf::Keyboard::Num7,
		sf::Keyboard::Num8,
		sf::Keyboard::Num9,
		sf::Keyboard::Num0,
		sf::Keyboard::Dash,
		sf::Keyboard::BackSpace,
		sf::Keyboard::Comma,
		sf::Keyboard::Period,
	};
	std::vector<char> symbols = {
		'q',
		'w',
		'e',
		'r',
		't',
		'y',
		'u',
		'i',
		'o',
		'p',
		'a',
		's',
		'd',
		'f',
		'g',
		'h',
		'j',
		'k',
		'l',
		'z',
		'x',
		'c',
		'v',
		'b',
		'n',
		'm',
		'1',
		'2',
		'3',
		'4',
		'5',
		'6',
		'7',
		'8',
		'9',
		'0',
		'-',
		'\b',
		',',
		'.',
	};
	std::vector<char> alt_symbols = {
		'Q',
		'W',
		'E',
		'R',
		'T',
		'Y',
		'U',
		'I',
		'O',
		'P',
		'A',
		'S',
		'D',
		'F',
		'G',
		'H',
		'J',
		'K',
		'L',
		'Z',
		'X',
		'C',
		'V',
		'B',
		'N',
		'M',
		'!',
		'@',
		'#',
		'$',
		'%',
		'^',
		'&',
		'*',
		'(',
		')',
		'_',
		'\b',
		'<',
		'>',
	};
	std::vector<KeyStat> key_stat;
	std::vector<std::chrono::time_point<std::chrono::steady_clock>> key_time_stamps;
	bool active = false;
	const int MAX_BUFFER_SIZE = 1000000000;
	int max_size = MAX_BUFFER_SIZE;

public:

	KeyboardBuffer() {
		key_stat.resize(keys.size());
		key_time_stamps.resize(keys.size());
		for (int i = 0; i < key_stat.size(); i++) {
			key_stat[i] = idle;
		}
		for (int i = 0; i < key_time_stamps.size(); i++) {
			key_time_stamps[i] = std::chrono::steady_clock::now();
		}

		auto keyboard_listener = [&]() {
			while (true) {
				if (active) {
					for (int i = 0; i < keys.size(); i++) {

						if (buffer.size() < max_size || symbols[i] == '\b') {
							if (sf::Keyboard::isKeyPressed(keys[i])) {
								if (key_stat[i] == idle) {
									if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
										if (symbols[i] != '\b') {
											buffer.push_back(alt_symbols[i]);
										}
										else if(buffer.size()){
											buffer.pop_back();
										}
									}
									else {
										if (symbols[i] != '\b') {
											buffer.push_back(symbols[i]);
										}
										else if (buffer.size()) {
											buffer.pop_back();
										}
									}
									key_stat[i] = first_pressed;
									key_time_stamps[i] = std::chrono::steady_clock::now();
								}
								else if (key_stat[i] == first_pressed) {
									if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - key_time_stamps[i]).count() > 400) {
										if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
											if (symbols[i] != '\b') {
												buffer.push_back(alt_symbols[i]);
											}
											else if (buffer.size()) {
												buffer.pop_back();
											}
										}
										else {
											if (symbols[i] != '\b') {
												buffer.push_back(symbols[i]);
											}
											else if (buffer.size()) {
												buffer.pop_back();
											}
										}
										key_stat[i] = pressed;
										key_time_stamps[i] = std::chrono::steady_clock::now();
									}
								}
								else {
									if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - key_time_stamps[i]).count() > 50) {
										if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
											if (symbols[i] != '\b') {
												buffer.push_back(alt_symbols[i]);
											}
											else if (buffer.size()) {
												buffer.pop_back();
											}
										}
										else {
											if (symbols[i] != '\b') {
												buffer.push_back(symbols[i]);
											}
											else if (buffer.size()) {
												buffer.pop_back();
											}
										}
										key_time_stamps[i] = std::chrono::steady_clock::now();
									}
								}
							}
							else {
								key_stat[i] = idle;
							}
						}
						else {
							key_stat[i] = idle;
						}
						
					}
				}
				Sleep(10);
			}
			
		};

		std::thread thread(keyboard_listener);
		thread.detach();
	}

	void activate() {
		active = true;
	}

	void deactivate() {
		active = true;
	}

	void clearBuffer() {
		buffer.clear();
	}

	std::string getBuffer() {
		return buffer;
	}

	void setMaxBufferSize(int size) {
		if (size <= 0 || size > MAX_BUFFER_SIZE) {
			max_size = MAX_BUFFER_SIZE;
		}
		else {
			max_size = size;
		}
	}
} keyboard_buffer;