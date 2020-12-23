#pragma once

#include <fstream>

#include "PhraseContainer.h"
#include "PlayerRPGProfile.h"
#include "DialogAdditionalInfo.h"
#include "ParserInput.h"

struct DialogInfo {
    struct Answer {
        std::wstring text;
        //PhraseContainer::PhraseType type;
        std::wstring type;
		std::wstring next_state_id;
        
        Answer(std::wstring text, std::wstring type, std::wstring next_state_id) : text(text), type(type), next_state_id(next_state_id) {}
    };
	bool is_player_turn = false;
	std::wstring main_text;
	std::vector<DialogInfo::Answer> answers; // if not players turn contains only "Continue"
};

struct Answer {
	std::wstring answer_text_id;
	std::wstring next_state_id;
    bool condition_not_flag = false;
	std::vector<std::pair<std::wstring, std::wstring>> conditions;
	std::vector<std::pair<std::wstring, std::wstring>> side_effects;

	Answer(
		std::wstring answer_text_id,
		std::wstring next_state_id,
		std::vector<std::pair<std::wstring, std::wstring>> conditions, 
		std::vector<std::pair<std::wstring, std::wstring>> side_effects) :
		answer_text_id(answer_text_id),
		next_state_id(next_state_id),
		conditions(conditions),
		side_effects(side_effects) {}
};

class NPCInfo {
public:
	enum ConversationStage {
		dialog_start,
		conversation_refuse,
		greetings_phrase,
		positive_greeting_reaction,
		neutral_greeting_reaction,
		negative_greeting_reaction,
		standart_question,
		joke,
		positive_joke_reaction,
		neutral_joke_reaction,
		negative_joke_reaction,
		job_selection,
		special_job_offer,
		standart_job,
		special_job,
		job_refuse,
		rumors_refuse,
		farewell,
		dialog_end
	};
private:

	// dialog state info

	bool special_job_available = false;
	bool base_job_available = true;
	bool job_question_passed = false;
	bool special_job_question_passed = false;
	bool rumors_available = false;
	bool rumors_question_passed = false;

	ConversationStage current_stage = dialog_start;
	std::map<std::wstring, std::vector<Answer>> dialog_state_list;   // map<state_name, state_answers_info
	std::map<std::wstring, std::vector<Answer>>::iterator cur_dialog_state;
	DialogInfo current_dialog_info;
	WorldFactionList faction = WORLD_FACTIONS_COUNT;

    Mission base_mission_info;
    Mission special_mission_info;

	std::wstring start_description_string;
	std::wstring personal_id;

	// personal qualities(from -100 to 100)

	int politeness = 0;
	int prejudices = 0;

	std::vector<std::wstring> buffer;

	void addAnswer(std::wstring phrase, std::wstring next_state_id) {
		buffer = phrase_container.getPhraseBuffer(phrase, politeness, personal_id);
		current_dialog_info.answers.push_back(DialogInfo::Answer(buffer[rand() * buffer.size() / (RAND_MAX + 1)], phrase, next_state_id));
	};
	void addMainText(std::wstring phrase) {
		current_dialog_info.main_text.clear();
		buffer = phrase_container.getPhraseBuffer(phrase, politeness, personal_id);
		current_dialog_info.main_text = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
	};

	std::map<std::wstring, std::vector<Answer>> getDialogStateFromFile(std::string path) {
		std::map<std::wstring, std::vector<Answer>> output;

		std::wifstream dialog_file;
		dialog_file.open(path);
		std::wstring input;
		
		while (true) {
			dialog_file >> input;
			if (input != L"dialog_node") {
				
			}
			if (input == L"file_end") {
				break;
			}
			std::wstring node_name;
			std::vector<Answer> answer_list;
			dialog_file >> input;
			if (input != L"node_name") {

			}
			dialog_file >> node_name;

			dialog_file >> input;
			while (input == L"answer") {
				Answer answer(L"", L"", {}, {});
				dialog_file >> input;
				if (input != L"answer_text_id") {

				}
				dialog_file >> answer.answer_text_id;

				dialog_file >> input;
				if (input != L"next_state_id") {

				}
				dialog_file >> answer.next_state_id;

				dialog_file >> input;
				if (input != L"conditions") {

				}
				dialog_file >> input;
				while (input == L">") {
					std::pair<std::wstring, std::wstring> condition;
					dialog_file >> condition.first;
					dialog_file >> condition.second;
					answer.conditions.push_back(condition);

					dialog_file >> input;
				}
				if (input != L"conditions_end") {

				}

				dialog_file >> input;
				if (input != L"side_effects") {

				}
				dialog_file >> input;
				while (input == L">") {
					std::pair<std::wstring, std::wstring> side_effect;
					dialog_file >> side_effect.first;
					dialog_file >> side_effect.second;
					answer.side_effects.push_back(side_effect);

					dialog_file >> input;
				}
				if (input != L"side_effects_end") {

				}

				answer_list.push_back(answer);

				dialog_file >> input;
			}
			if (input != L"dialog_node_end") {

			}

			output.insert(std::pair<std::wstring, std::vector<Answer>>(node_name, answer_list));
		}
		return output;
	}

    void setDefaultSettings() {
        politeness = rand() * 200 / (RAND_MAX + 1) - 100;
        prejudices = rand() * 200 / (RAND_MAX + 1) - 100;

        faction = WorldFactionList(rand() % WORLD_FACTIONS_COUNT);

        std::vector<std::wstring> buffer;

        /*buffer = phrase_container.getPhraseBuffer(L"start_description", politeness, personal_id);
        start_description_string = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        current_dialog_info.main_text.clear();
        current_dialog_info.main_text = start_description_string;
        current_dialog_info.answers.clear();

        current_dialog_info.answers.push_back(getContinueAnswer());
        current_dialog_info.is_player_turn = false;*/


		//// new part

		/*dialog_state_list = 
		{
			{ L"start_description", std::vector<Answer>
				({
					Answer(L"continue_phrase_GUI", L"greetings_phrase_npc", {{L"fame_greater", L"-50"}}, {}),
					Answer(L"continue_phrase_GUI", L"conversation_refuse", {{L"fame_notgreater", L"-50"}}, {})
				}) 
			},
			{ L"greetings_phrase_npc", std::vector<Answer>
				({
					Answer(L"positive_greetings_answer_player", L"positive_greetings_reaction_npc",{},{}),
					Answer(L"neutral_greetings_answer_player", L"neutral_greetings_reaction_npc",{},{}),
					Answer(L"negative_greetings_answer_player", L"negative_greetings_reaction_npc",{},{}),
					Answer(L"farewell_player", L"dialog_end",{},{}),
				}) 
			},
			{ L"dialog_end", std::vector<Answer>
				({
					Answer(L"continue_phrase_GUI", L"start_description",{},{})
				})
			},
			{ L"conversation_refuse", std::vector<Answer>
				({
					Answer(L"continue_phrase_GUI", L"dialog_end",{},{})
				})
			},
		};*/
		cur_dialog_state = dialog_state_list.find(L"start_description");

		initState(L"start_description");
    }

	bool isValidCondition(std::pair<std::wstring, std::wstring> condition) {
		if (condition.first == L"special_job_available") {
			return condition.second == L"1";
		}
		if (condition.first == L"base_job_available") {
			return condition.second == L"1";
		}
		if (condition.first == L"special_job_accepted") {
			return condition.second == L"1";
		}
		if (condition.first == L"base_job_accepted") {
			return condition.second == L"1";
		}
		if (condition.first == L"rumor_available") {
			return condition.second == L"1";
		}
		if (condition.first == L"rumor_accepted") {
			return condition.second == L"1";
		}
		if (condition.first == L"fame_lower") {
			int fame = prejudices + rpg_profile.getFactionFame(faction) + rpg_profile.getGlobalFame();
			return fame < std::stoi(condition.second);
		}
		if (condition.first == L"fame_notgreater") {
			int fame = prejudices + rpg_profile.getFactionFame(faction) + rpg_profile.getGlobalFame();
			return fame <= std::stoi(condition.second);
		}
		if (condition.first == L"fame_equal") {
			int fame = prejudices + rpg_profile.getFactionFame(faction) + rpg_profile.getGlobalFame();
			return fame == std::stoi(condition.second);
		}
		if (condition.first == L"fame_notlower") {
			int fame = prejudices + rpg_profile.getFactionFame(faction) + rpg_profile.getGlobalFame();
			return fame >= std::stoi(condition.second);
		}
		if (condition.first == L"fame_greater") {
			int fame = prejudices + rpg_profile.getFactionFame(faction) + rpg_profile.getGlobalFame();
			return fame > std::stoi(condition.second);
		}

        std::wstring var_name;
        std::size_t pos;
        enum {
            more_or_equal,
            more,
            less_or_equal,
            less,
            equal,
            not_equal
        } mode;

        if ((pos = condition.first.find(L"_more_or_equal")) != std::string::npos) {
            var_name = condition.first.substr(0, pos);
            mode = more_or_equal;
        }
        else if ((pos = condition.first.find(L"_more")) != std::string::npos) {
            var_name = condition.first.substr(0, pos);
            mode = more;
        }
        else if ((pos = condition.first.find(L"_less_or_equal")) != std::string::npos) {
            var_name = condition.first.substr(0, pos);
            mode = less_or_equal;
        }
        else if ((pos = condition.first.find(L"_less")) != std::string::npos) {
            var_name = condition.first.substr(0, pos);
            mode = less;
        }
        else if ((pos = condition.first.find(L"_not_equal")) != std::string::npos) {
            var_name = condition.first.substr(0, pos);
            mode = not_equal;
        }
        else if ((pos = condition.first.find(L"_equal")) != std::string::npos) {
            var_name = condition.first.substr(0, pos);
            mode = equal;
        }

        std::wstring var = dialog_additional_info.getData(var_name);
        if (var.size() != 0) {
            int num = std::atoi(wstringToString(var).c_str());
            if (num == 0 && var != L"0") {
                if (mode == equal) {
                    return var == condition.second;
                }
                if (mode == not_equal) {
                    return var != condition.second;
                }
                return false;
            }
            int num2 = std::atoi(wstringToString(condition.second).c_str());
            if (num2 == 0 && condition.second != L"0") {
                return false;
            }
            switch (mode) {
            case equal:
                return num == num2;
            case not_equal:
                return num != num2;
            case more_or_equal:
                return num >= num2;
            case more:
                return num > num2;
            case less_or_equal:
                return num <= num2;
            case less:
                return num < num2;
            default:
                return false;
            }
        }
        else {
            return false;
        }
	}

    void initState(std::wstring state) {
        auto new_state = dialog_state_list.find(state);

        if (new_state != dialog_state_list.end()) {
            addMainText(state);

            current_dialog_info.answers.clear();
            for (int i = 0; i < new_state->second.size(); i++) {
                bool answer_valid = true;
                for (auto cond = new_state->second[i].conditions.begin(); cond != new_state->second[i].conditions.end(); cond++) {
                    if (!isValidCondition(*cond)) {
                        answer_valid = false;
                    }
                }
                if (answer_valid ^ new_state->second[i].condition_not_flag) {
                    addAnswer(new_state->second[i].answer_text_id, new_state->second[i].next_state_id);
                }
            }

            cur_dialog_state = new_state;
        }
    }

public:


	NPCInfo(WorldFactionList faction, std::wstring personal_id) : faction(faction), personal_id(personal_id), base_mission_info(0), special_mission_info(0) {
        setDefaultSettings();
		
		dialog_state_list = getDialogStateFromFile("dialog_config\\" + wstringToString(personal_id) + ".txt");

		cur_dialog_state = dialog_state_list.find(L"start_description");
	}

    NPCInfo(WorldFactionList faction) : NPCInfo(faction, L"base_dialog") {
        std::vector<std::wstring> buffer = phrase_container.getPersonalIdList();
        personal_id = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        setDefaultSettings();
    }

	std::wstring getCurrentStage() {
		std::wstring output;
		if (cur_dialog_state != dialog_state_list.end()) {
			output = cur_dialog_state->first;
		}
		return output;
	}

	void nextTurn(std::wstring next_state_id) {
		if (cur_dialog_state != dialog_state_list.end()) {
			initState(next_state_id);
		}
	}

	DialogInfo getDialogInfo() {
		return current_dialog_info;
	}

	void changeBaseMission(Mission new_base_mission) {
		base_mission_info.clear();
		base_mission_info = new_base_mission;
	}

	void changeSpecialMission(Mission new_special_mission) {
		special_mission_info.clear();
		special_mission_info = new_special_mission;
	}
};