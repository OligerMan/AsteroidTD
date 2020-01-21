#pragma once

#include "SFML\Graphics.hpp"

#include "NPCInfo.h"

class DialogVisualController {
    sf::Font main_font;
    sf::Text main_text;
    sf::Text answer_text;

    int max_dialog_window_width = 1200;
    int max_dialog_window_height = 800;
    int border_width = 100;
    int answer_border_top = 100;
    int answer_border = 35;

    void setFormatString(std::string raw_string, sf::Text & test_text, unsigned int width) {
        std::string output;

        for (int i = 0; i < raw_string.size(); i++) {
            std::string word;
            while (i < raw_string.size() && raw_string[i] != ' ') {
                word.push_back(raw_string[i]);
                i++;
            }
            test_text.setString(output + " " + word);
            if (test_text.getGlobalBounds().width < width) {
                output.append(" " + word);
            }
            else {
                output.append("\n" + word);
            }
        }

        test_text.setString(output);
    }

public:

    DialogVisualController(sf::RenderWindow & window) {
        main_font.loadFromFile("a_Alterna.ttf");

        main_text.setPosition(sf::Vector2f(window.getView().getSize().x / 2, 150 * window.getView().getSize().y / 1080));
        main_text.setFillColor(sf::Color::White);
        main_text.setOutlineColor(sf::Color::Black);
        main_text.setOutlineThickness(1);
        main_text.setCharacterSize(50 * window.getView().getSize().y / 1080);
        main_text.setFont(main_font);

        answer_text.setPosition(sf::Vector2f(window.getView().getSize().x / 2, 150 * window.getView().getSize().y / 1080));
        answer_text.setFillColor(sf::Color::White);
        answer_text.setOutlineColor(sf::Color::Black);
        answer_text.setOutlineThickness(1);
        answer_text.setCharacterSize(50 * window.getView().getSize().y / 1080);
        answer_text.setFont(main_font);
    }

    void processDialog(DialogInfo info, sf::RenderWindow & window, int selected_answer) {
        sf::RectangleShape dialog_back(sf::Vector2f(max_dialog_window_width + border_width, max_dialog_window_height + border_width));
        dialog_back.setFillColor(sf::Color(30, 30, 30));
        sf::Vector2f view_size = window.getView().getSize();
        sf::Vector2f view_center = sf::Vector2f(view_size.x / 2, view_size.y / 2);
        sf::Vector2f dialog_window_shift = -sf::Vector2f((max_dialog_window_width) / 2, (max_dialog_window_height) / 2);

        dialog_back.setPosition(view_center - sf::Vector2f((border_width) / 2, (border_width) / 2) + dialog_window_shift);
        window.draw(dialog_back);

        setFormatString(info.main_text, main_text, max_dialog_window_width);
        main_text.setPosition(view_center + dialog_window_shift);
        window.draw(main_text);

        int answer_shift = 0;
        for (int i = 0; i < info.answers.size(); i++) {
            setFormatString((i == selected_answer ? "> " : "") + info.answers[i], answer_text, max_dialog_window_width - 2 * answer_border);
            answer_text.setPosition(sf::Vector2f(answer_border, main_text.getGlobalBounds().height + answer_border_top + answer_border + answer_shift) + dialog_window_shift + view_center);
            answer_shift += answer_text.getGlobalBounds().height + answer_border_top;
            window.draw(answer_text);
        }
    }
};