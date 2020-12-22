#pragma once
#include <vector>
#include <string>


#include <SFML/Audio.hpp>

class SoundManager {
    std::vector<sf::SoundBuffer> sound_buf_list;
    std::vector<sf::Sound> sound_list;

    std::vector<std::wstring> sound_path_list = {
        L"shoot.ogg"
    };

public:

    enum {
        shoot,

        SOUND_COUNT
    };

    SoundManager(std::wstring path) {
        for (int i = 0; i < SOUND_COUNT; i++) {

        }
    }
};