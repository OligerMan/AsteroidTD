#pragma once

#include "SFML\Graphics.hpp"

void setFormatString(std::string raw_string, sf::Text & test_text, unsigned int width, unsigned int height, unsigned int base_character_size, sf::RenderWindow & window) {
	std::string output;

	for (int i = 0; i < raw_string.size(); i++) {
		std::string word;
		while (i < raw_string.size() && raw_string[i] != ' ') {
			word.push_back(raw_string[i]);
			i++;
		}
		test_text.setString(output + " " + word);
		if (test_text.getGlobalBounds().width < width * window.getView().getSize().y / 1080) {
			output.append(" " + word);
		}
		else {
			output.append("\n" + word);
		}
	}

	test_text.setString(output);
	test_text.setCharacterSize(base_character_size * window.getView().getSize().y / 1080);
	while (test_text.getGlobalBounds().height > height * window.getView().getSize().y / 1080 && test_text.getCharacterSize() > 10) {
		test_text.setCharacterSize(test_text.getCharacterSize() - 1);
	}
}

void setFormatString(std::wstring raw_string, sf::Text & test_text, unsigned int width, unsigned int height, unsigned int base_character_size, sf::RenderWindow & window) {
    std::wstring output;

    for (int i = 0; i < raw_string.size(); i++) {
        std::wstring word;
        while (i < raw_string.size() && raw_string[i] != ' ') {
            word.push_back(raw_string[i]);
            i++;
        }
        test_text.setString(output + L" " + word);
        if (test_text.getGlobalBounds().width < width * window.getView().getSize().y / 1080) {
            output.append(L" " + word);
        }
        else {
            output.append(L"\n" + word);
        }
    }

    test_text.setString(output);
    test_text.setCharacterSize(base_character_size * window.getView().getSize().y / 1080);
    while (test_text.getGlobalBounds().height > height * window.getView().getSize().y / 1080 && test_text.getCharacterSize() > 10) {
        test_text.setCharacterSize(test_text.getCharacterSize() - 1);
    }
}