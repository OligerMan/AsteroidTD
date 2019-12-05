#pragma once

#include <vector>
#include <string>
#include <thread>
#include <random>
#include <Windows.h>

#include <SFML\Audio.hpp>

#include "FileSearch.h"

class MusicManager {
	std::vector<sf::Music *> playlist;
	std::vector<std::string> track_name;
	int current_track = 0;
	bool play_state = true;

	std::random_device r;
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution;

	bool inRange(int track_num) {
		return ((track_num >= 0) && (track_num < playlist.size()));
	}

public:

	MusicManager(std::string path) {
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

	void setVolume(float volume) {
		playlist[current_track]->setVolume(volume);
	}

	float getVolume() {
		return playlist[current_track]->getVolume();
	}

	void launchMusicWorker() {
		auto music_worker = [&]() {
			while (true) {
				if (play_state && !isPlaying() && playlist.size()) {
					if (playlist.size()) {
						int prev_track = current_track;
						while (current_track == prev_track) {
							current_track = distribution(generator) - 1;
						}
						playlist[current_track]->setVolume(30.0f);
						play_state = true;

						std::cout << "New track is " << track_name[current_track] << std::endl;
						playlist[current_track]->play();
					}
					else {
						play_state = false;
					}
				}
				Sleep(200);
			}
		};

		std::thread thread(music_worker);
		thread.detach();
	}
};