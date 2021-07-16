#pragma once

#include <vector>
#include <string>
#include <thread>
#include <random>

#if  defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#elif _WIN32
#include <Windows.h>
#else
#endif

#include <SFML/Audio.hpp>

#include "FileSearch.h"
#include "Settings.h"

class MusicManager {
	std::vector<sf::Music *> playlist;
	std::vector<std::string> track_name;
	int current_track = 0;
	bool play_state = true;
	int current_volume = 30;

	std::random_device r;
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution;

	bool inRange(int track_num) {
		return ((track_num >= 0) && (track_num < playlist.size()));
	}

	bool working = true;
	bool main_cycle_down = false;

public:

	MusicManager(std::string path) {
        current_volume = settings.getVolume();
		track_name = *getFileList(path);
		playlist.resize(track_name.size());
		for (int i = 0; i < playlist.size(); i++) {
			playlist[i] = new sf::Music();
		}
		std::cout << "Loading music list" << std::endl;
		for (int i = 0; i < track_name.size(); i++) {
			std::cout << "Loading track " << track_name[i] << std::endl;
			playlist[i]->openFromFile(path + "/" + track_name[i]);
		}
		generator = std::default_random_engine(r());
		distribution = std::uniform_int_distribution<int>(1, playlist.size());
	}

    void shutdown() {
        working = false;
        while (!main_cycle_down) {
#if  defined(__linux__) || defined(__APPLE__)
            usleep(10000);
#elif _WIN32
			Sleep(10);
#endif
        }
    }

	~MusicManager() {
        shutdown();
	}

	bool isTrackPlaying(int track_num) {
		return (inRange(track_num) && (playlist[track_num]->getStatus() == sf::SoundSource::Status::Playing));
	}

	bool isPlaying() {
		return isTrackPlaying(current_track);
	}

	void pause() {
		play_state = false;
		playlist[current_track]->pause();
	}

	void play() {
		play_state = true;
		playlist[current_track]->play();
	}

	void setVolume(int volume) {
		playlist[current_track]->setVolume(volume);
        settings.setVolume(volume);
		current_volume = volume;
	}

	float getVolume() {
		return current_volume;
	}

	void launchMusicWorker() {
		auto music_worker = [&]() {
			while (working) {
				if (play_state && !isPlaying() && playlist.size()) {
					if (playlist.size()) {
						int prev_track = current_track;
						while (current_track == prev_track) {
							current_track = distribution(generator) - 1;
						}
						playlist[current_track]->setVolume(current_volume);
						play_state = true;

						std::cout << "New track is " << track_name[current_track] << std::endl;
						playlist[current_track]->play();
					}
					else {
						play_state = false;
					}
				}
#if  defined(__linux__) || defined(__APPLE__)
				usleep(200000);
#elif _WIN32
				Sleep(200);
#endif
			}
			main_cycle_down = true;
		};

		std::thread thread(music_worker);
		thread.detach();
	}
};