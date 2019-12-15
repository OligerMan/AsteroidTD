#pragma once

#include <thread>
#include <vector>
#include <string>
#include <SFML\Network.hpp>
#include <Windows.h>

#include "PlayerStat.pb.h"
#include "GameConstants.h"
#include "Settings.h"
#include "GameStatus.h"

class OnlineRank {
	std::vector<PlayerKillStat> top5kill;
	std::vector<PlayerKillStat> round5kill;
	std::vector<PlayerSurviveStat> top5survive;
	std::vector<PlayerSurviveStat> round5survive;

	int online_count = 1;

	sf::TcpSocket socket;
	std::thread * rank_update_worker = nullptr;

	// user info
	int kills_amount = 0;
	int gameplay_time = 0; // in milliseconds
	bool dead = true;
	bool online = true;

public:

	void selfRankUpdate(bool live) {
		if (!live) {
			std::cout << "Sending post-game statistics(" << kills_amount << " kills, " << gameplay_time << " time)" << std::endl;
			dead = true;
		}
		PlayerInfo info;
		info.set_kills(kills_amount);
		info.set_time_survived(gameplay_time);
		info.set_nickname(settings.getNickname());
		std::hash<std::string> hash_str;
		info.set_hash(hash_str(consts.getBalanceVersion()));
		info.set_live(live);
		char data[10000];
		std::string str = info.SerializeAsString();
		for (int i = 0; i < str.size(); i++) {
			data[i] = str[i];
		}
		data[str.size()] = 0;
		socket.send(data, str.size());
	}

	void launchUpdateWorker() {
		auto rank_updater = [&]() {
			while (game_status == nickname_enter && !settings.getNickname().size()) { Sleep(1000); }
			if (socket.connect("oliger.ddns.net", 50000) != sf::Socket::Done) {
				std::cout << "Socket connect fail" << std::endl;
				return;
			}
			std::cout << "Socket for ranking connected" << std::endl;
			std::size_t received;
			const int data_max_size = 10000;
			char data[data_max_size];
			for (int i = 0; i < settings.getNickname().size(); i++) {
				data[i] = settings.getNickname()[i];
			}
			if (settings.getNickname().size() == 0) {
				socket.disconnect();
				return;
			}
			socket.send(data, settings.getNickname().size());

			RankInfo rank_info;

			while (online) {
				for (int i = 0; i < data_max_size; i++) {
					data[i] = 0;
				}
				if (socket.receive(data, data_max_size, received) != sf::Socket::Done)
				{
					std::cout << "Data receive error" << std::endl;
					break;
				}

				rank_info.ParseFromArray(data, received);

				online_count = rank_info.online_count();

				top5kill.clear();
				round5kill.clear();
				top5survive.clear();
				round5survive.clear();

				for (int i = 0; i < rank_info.top5kill().size(); i++) {
					top5kill.push_back(rank_info.top5kill()[i]);
				}
				for (int i = 0; i < rank_info.round5kill().size(); i++) {
					round5kill.push_back(rank_info.round5kill()[i]);
				}
				for (int i = 0; i < rank_info.top5survive().size(); i++) {
					top5survive.push_back(rank_info.top5survive()[i]);
				}
				for (int i = 0; i < rank_info.round5survive().size(); i++) {
					round5survive.push_back(rank_info.round5survive()[i]);
				}
			}
			online = true;
		};

		std::thread thread(rank_updater);
		thread.detach();
	}

	void reboot() {
		online = false;
		while (!online) { Sleep(10); }
		launchUpdateWorker();
	}

	void launchSelfRankUpdateWorker() {
		auto self_rank_updater = [&]() {
			while (game_status == nickname_enter) { Sleep(1000); }
			while (true) {
				if (!dead && online) {
					selfRankUpdate(true);
				}
				Sleep(1000);
			}
		};

		std::thread thread(self_rank_updater);
		thread.detach();
	}

	std::vector<std::string> getTopKillList() {
		std::vector<std::string> output;
		output.push_back("Top killers list");
		for (int i = 0; i < top5kill.size(); i++) {
			output.push_back(std::to_string((int)top5kill[i].kills_place()) + ". " + top5kill[i].nickname() + " >> " + std::to_string(top5kill[i].kills()) + " kills");
		}
		
		if (top5kill.size() == 6) {
			output.push_back("  ...  ");
			for (int i = 0; i < round5kill.size(); i++) {
				output.push_back(round5kill[i].kills_place() + ". " + round5kill[i].nickname() + " >> " + std::to_string(round5kill[i].kills()) + " kills");
			}
		}
		return output;
	}

	std::vector<std::string> getTopSurviveTimeList() {
		std::vector<std::string> output;

		output.push_back("Top survivors list");
		for (int i = 0; i < top5survive.size(); i++) {
			output.push_back(std::to_string(top5survive[i].time_survived_place()) + ". " + top5survive[i].nickname() + " >> " + std::to_string((int)top5survive[i].time_survived() / 60000) + "m " + std::to_string((int)top5survive[i].time_survived() / 1000 % 60) + "s");
		}

		if (top5survive.size() == 6) {
			output.push_back("  ...  ");
			for (int i = 0; i < round5survive.size(); i++) {
				output.push_back(std::to_string(round5survive[i].time_survived_place()) + ". " + round5survive[i].nickname() + " >> " + std::to_string(round5survive[i].time_survived()));
			}
		}
		return output;
	}

	void addKills(int new_kills) {
		kills_amount += new_kills;
	}

	void addGameplayTime(int new_time_duration) {
		gameplay_time += new_time_duration;
	}

	void resetUserInfo() {
		dead = false;
		kills_amount = 0;
		gameplay_time = 0;
	}
}rank;