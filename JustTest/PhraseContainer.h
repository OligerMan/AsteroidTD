#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <codecvt>
#include <locale>

#include "Settings.h"
#include "ParserInput.h"
#include "FileSearch.h"

struct Phrase {
	std::wstring phrase, id;
	int min_politeness = -100;
	int max_politeness = 100;
};

class PhraseContainer {
public:
	enum PhraseType {
		start_description,
		conversation_refuse_npc,

		greetings_phrase_npc,

		positive_greetings_answer_player,
		neutral_greetings_answer_player,
		negative_greetings_answer_player,

		positive_greetings_reaction_npc,
		neutral_greetings_reaction_npc,
		negative_greetings_reaction_npc,

		standart_question_npc,

		job_question_player,
		rumors_question_player,

		random_joke_player,
		random_positive_joke_reaction_npc,
		random_neutral_joke_reaction_npc,
		random_negative_joke_reaction_npc,

		rumors_refuse_npc,

		base_job_refuse_npc,
		special_job_refuse_npc,
		special_job_question_player,
		base_job_question_player,
		job_refuse_player,
		job_accept_player,

		farewell_player,
		farewell_npc,

		dialog_end_npc,

        courier_mission_completed_npc,
        defence_mission_completed_npc,

        continue_phrase_GUI,
        reroll_phrase_GUI,
        joke_phrase_GUI,
        start_inf_mode_GUI,
        start_adv_mode_GUI,
        settings_menu_GUI,
        shutdown_GUI,
        keyboard_press_title_GUI,
        gamepad_press_title_GUI,
        setting_nickname_title_GUI,
        setting_ranking_title_GUI,
        setting_win_width_title_GUI,
        setting_win_height_title_GUI,
        setting_local_file_title_GUI,
        setting_exit_title_GUI,
        setting_discard_title_GUI,
        keyboard_nickname_title_GUI,
        gamepad_nickname_title_GUI,
        nickname_enter_title_GUI,

        gold_sign_GUI,
        research_sign_GUI,
		skills_build_sign_GUI,
		skills_ability_sign_GUI,
		skills_interact_sign_GUI,
		new_wave_sign_GUI,
        message_sign_GUI,

        try_again_title_GUI,
        go_back_to_menu_title_GUI,
        continue_game_title_GUI,
        go_to_menu_title_GUI,
        go_to_desktop_title_GUI,

        start_game_sign_GUI,

        aster_basic_GUI,
        aster_gold_inter_GUI,
        aster_iron_inter_GUI,
        aster_susp_flat_GUI,

        aster_str_crack_GUI,
        aster_ord_wealthy_GUI,
        aster_poor_mount_GUI,
        aster_wealthy_crack_GUI,
        aster_ord_mount_GUI,
        aster_str_poor_GUI,

        aster_swamp_gold_GUI,
        aster_unstab_expl_GUI,
        aster_old_lab_GUI,
        aster_lava_surf_GUI,

        aster_drone_GUI,
        aster_rocket_GUI,
        aster_anc_lab_GUI,
        aster_anc_mine_GUI,

		PHRASE_TYPE_COUNT
	};

private:

	std::vector<std::vector<Phrase>> phrase_buffer;
	std::vector<std::wstring> phrase_type_strings;
	std::vector<std::wstring> personal_id_list;

	int is_phrase_type_exists(std::wstring example) {
		for (int i = 0; i < PHRASE_TYPE_COUNT; i++) {
			if (phrase_type_strings[i] == example) {
				return i;
			}
		}
		return -1;
	}

	int is_personal_id_exists(std::wstring example) {
		for (int i = 0; i < personal_id_list.size(); i++) {
			if (personal_id_list[i] == example) {
				return i;
			}
		}
		return -1;
	}

public:

	PhraseContainer() {
		phrase_type_strings.resize(PHRASE_TYPE_COUNT);
		phrase_buffer.resize(PHRASE_TYPE_COUNT);

		phrase_type_strings[start_description] = L"start_description";
		phrase_type_strings[conversation_refuse_npc] = L"conversation_refuse_npc";

		phrase_type_strings[greetings_phrase_npc] = L"greetings_phrase_npc";

		phrase_type_strings[positive_greetings_answer_player] = L"positive_greetings_answer_player";
		phrase_type_strings[neutral_greetings_answer_player] = L"neutral_greetings_answer_player";
		phrase_type_strings[negative_greetings_answer_player] = L"negative_greetings_answer_player";

		phrase_type_strings[positive_greetings_reaction_npc] = L"positive_greetings_reaction_npc";
		phrase_type_strings[neutral_greetings_reaction_npc] = L"neutral_greetings_reaction_npc";
		phrase_type_strings[negative_greetings_reaction_npc] = L"negative_greetings_reaction_npc";

		phrase_type_strings[standart_question_npc] = L"standart_question_npc";

		phrase_type_strings[job_question_player] = L"job_question_player";
		phrase_type_strings[rumors_question_player] = L"rumors_question_player";

		phrase_type_strings[random_joke_player] = L"random_joke_player";
		phrase_type_strings[random_positive_joke_reaction_npc] = L"random_positive_joke_reaction_npc";
		phrase_type_strings[random_neutral_joke_reaction_npc] = L"random_neutral_joke_reaction_npc";
		phrase_type_strings[random_negative_joke_reaction_npc] = L"random_negative_joke_reaction_npc";

		phrase_type_strings[rumors_refuse_npc] = L"rumors_refuse_npc";

		phrase_type_strings[base_job_refuse_npc] = L"base_job_refuse_npc";
		phrase_type_strings[special_job_refuse_npc] = L"special_job_refuse_npc";
		phrase_type_strings[special_job_question_player] = L"special_job_question_player";
		phrase_type_strings[base_job_question_player] = L"base_job_question_player";
		phrase_type_strings[job_refuse_player] = L"job_refuse_player";
		phrase_type_strings[job_accept_player] = L"job_accept_player";

		phrase_type_strings[farewell_player] = L"farewell_player";
		phrase_type_strings[farewell_npc] = L"farewell_npc";

		phrase_type_strings[dialog_end_npc] = L"dialog_end";

        phrase_type_strings[courier_mission_completed_npc] = L"courier_mission_completed_npc";
        phrase_type_strings[defence_mission_completed_npc] = L"defence_mission_completed_npc";

        phrase_type_strings[continue_phrase_GUI] = L"continue_phrase_GUI";
        phrase_type_strings[reroll_phrase_GUI] = L"reroll_phrase_GUI";
        phrase_type_strings[joke_phrase_GUI] = L"joke_phrase_GUI";
        phrase_type_strings[start_inf_mode_GUI] = L"start_inf_mode_GUI";
        phrase_type_strings[start_adv_mode_GUI] = L"start_adv_mode_GUI";
        phrase_type_strings[settings_menu_GUI] = L"settings_menu_GUI";
        phrase_type_strings[shutdown_GUI] = L"shutdown_GUI";
        phrase_type_strings[keyboard_press_title_GUI] = L"keyboard_press_title_GUI";
        phrase_type_strings[gamepad_press_title_GUI] = L"gamepad_press_title_GUI";
        phrase_type_strings[setting_nickname_title_GUI] = L"setting_nickname_title_GUI";
        phrase_type_strings[setting_ranking_title_GUI] = L"setting_ranking_title_GUI";
        phrase_type_strings[setting_win_width_title_GUI] = L"setting_win_width_title_GUI";
        phrase_type_strings[setting_win_height_title_GUI] = L"setting_win_height_title_GUI";
        phrase_type_strings[setting_local_file_title_GUI] = L"setting_local_file_title_GUI";
        phrase_type_strings[setting_exit_title_GUI] = L"setting_exit_title_GUI";
        phrase_type_strings[setting_discard_title_GUI] = L"setting_discard_title_GUI";
        phrase_type_strings[keyboard_nickname_title_GUI] = L"keyboard_nickname_title_GUI";
        phrase_type_strings[gamepad_nickname_title_GUI] = L"gamepad_nickname_title_GUI";
        phrase_type_strings[nickname_enter_title_GUI] = L"nickname_enter_title_GUI";

        phrase_type_strings[gold_sign_GUI] = L"gold_sign_GUI";
		phrase_type_strings[research_sign_GUI] = L"research_sign_GUI";
		phrase_type_strings[skills_build_sign_GUI] = L"skills_build_sign_GUI";
		phrase_type_strings[skills_ability_sign_GUI] = L"skills_ability_sign_GUI";
		phrase_type_strings[skills_interact_sign_GUI] = L"skills_interact_sign_GUI";
        phrase_type_strings[new_wave_sign_GUI] = L"new_wave_sign_GUI";
        phrase_type_strings[message_sign_GUI] = L"message_sign_GUI";

        phrase_type_strings[try_again_title_GUI] = L"try_again_title_GUI";
        phrase_type_strings[go_back_to_menu_title_GUI] = L"go_back_to_menu_title_GUI";
        phrase_type_strings[continue_game_title_GUI] = L"continue_game_title_GUI";
        phrase_type_strings[go_to_menu_title_GUI] = L"go_to_menu_title_GUI";
        phrase_type_strings[go_to_desktop_title_GUI] = L"go_to_desktop_title_GUI";
        phrase_type_strings[go_to_desktop_title_GUI] = L"go_to_desktop_title_GUI";

        phrase_type_strings[start_game_sign_GUI] = L"start_game_sign_GUI";

        phrase_type_strings[aster_basic_GUI] = L"aster_basic_GUI";
        phrase_type_strings[aster_gold_inter_GUI] = L"aster_gold_inter_GUI";
        phrase_type_strings[aster_iron_inter_GUI] = L"aster_iron_inter_GUI";
        phrase_type_strings[aster_susp_flat_GUI] = L"aster_susp_flat_GUI";

        phrase_type_strings[aster_str_crack_GUI] = L"aster_str_crack_GUI";
        phrase_type_strings[aster_ord_wealthy_GUI] = L"aster_ord_wealthy_GUI";
        phrase_type_strings[aster_poor_mount_GUI] = L"aster_poor_mount_GUI";
        phrase_type_strings[aster_wealthy_crack_GUI] = L"aster_wealthy_crack_GUI";
        phrase_type_strings[aster_ord_mount_GUI] = L"aster_ord_mount_GUI";
        phrase_type_strings[aster_str_poor_GUI] = L"aster_str_poor_GUI";

        phrase_type_strings[aster_swamp_gold_GUI] = L"aster_swamp_gold_GUI";
        phrase_type_strings[aster_unstab_expl_GUI] = L"aster_unstab_expl_GUI";
        phrase_type_strings[aster_old_lab_GUI] = L"aster_old_lab_GUI";
        phrase_type_strings[aster_lava_surf_GUI] = L"aster_lava_surf_GUI";

        phrase_type_strings[aster_drone_GUI] = L"aster_drone_GUI";
        phrase_type_strings[aster_rocket_GUI] = L"aster_rocket_GUI";
        phrase_type_strings[aster_anc_lab_GUI] = L"aster_anc_lab_GUI";
        phrase_type_strings[aster_anc_mine_GUI] = L"aster_anc_mine_GUI";
	}

	/* 
	file structure:

	dialog_file_start

	phrase_start
	string |<string between straight slashes>|
	conditions_start
	<condition_name> <condition_value>
	... other conditions
	conditions_end
	id <personal id>
	phrase_type <type>
	phrase_end

	... other phrases

	dialog_file_end
	*/
	void parseFromFile(std::string path) {
		std::wifstream dialog_file;

		dialog_file.open(path);
        dialog_file.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
		std::wstring input;
		dialog_file >> input;
		if (input != L"dialog_file_start") {
			if (settings.isErrorOutputEnabled()) {
				std::cout << "Dialog file is corrupted" << std::endl;
			}
			dialog_file.close();
			return;
		}

		while (true) {
			dialog_file >> input;
			if (input == L"dialog_file_end") {
				break;
			}
			if (input != L"phrase_start") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Phrase start expected" << std::endl;
				}
				dialog_file.close();
				return;
			}

			Phrase new_phrase;

			dialog_file >> input;
			if (input != L"string") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "String marker expected" << std::endl;
				}
				dialog_file.close();
				return;
			}
            wchar_t input_char = ' ';
			while (dialog_file.get() != L'\|') {
            }
			input.clear();
			while (true) {
                input_char = dialog_file.get();
				if (input_char == L'\|') {
					break;
				}
				if (input_char == L'@') {
					input_char = L'\n';
				}
				input.push_back(input_char);
			}

			new_phrase.phrase = input;

			dialog_file >> input;
			if (input != L"conditions_start") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Conditions marker expected" << std::endl;
				}
				dialog_file.close();
				return;
			}

			while (true) {
				dialog_file >> input;
				if (input == L"conditions_end") {
					break;
				}
				if (input == L"") {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Conditions block corrupted" << std::endl;
					}
					dialog_file.close();
					return;
				}
				if (input == L"min_politeness") {
					new_phrase.min_politeness = getInt(dialog_file);
				}
				else if (input == L"max_politeness") {
					new_phrase.max_politeness = getInt(dialog_file);
				}
				else {
					dialog_file >> input;
				}
				
			}

			dialog_file >> input;
			if (input != L"id") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "ID marker expected" << std::endl;
				}
				dialog_file.close();
				return;
			}
			dialog_file >> new_phrase.id;

			dialog_file >> input;
			if (input != L"phrase_type") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Phrase type marker expected" << std::endl;
				}
				dialog_file.close();
				return;
			}
			dialog_file >> input;
			int type = is_phrase_type_exists(input);
			if (type != -1) {
				phrase_buffer[type].push_back(new_phrase);
				if (is_personal_id_exists(new_phrase.id) == -1) {
					personal_id_list.push_back(new_phrase.id);
				}
			}

			dialog_file >> input;
			if (input != L"phrase_end") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Phrase end expected" << std::endl;
				}
				dialog_file.close();
				return;
			}
		}
	}

    void parseFromFolder(std::string path) {
        std::vector<std::string> * path_list = getFileList(path);
        for (auto i = path_list->begin(); i != path_list->end(); i++) {
            parseFromFile(path + "\\" + *i);
        }
    }

	std::vector<std::wstring> getPhraseBuffer(PhraseType type, int politeness, std::wstring id = L"") {
		std::vector<std::wstring> output;
		if (phrase_buffer.size() >= type) {
			for (int i = 0; i < phrase_buffer[type].size(); i++) {
				if (politeness >= phrase_buffer[type][i].min_politeness && politeness <= phrase_buffer[type][i].max_politeness) {
					if (id.empty() || id == L"common" || id == phrase_buffer[type][i].id || phrase_buffer[type][i].id == L"common") {
						output.push_back(phrase_buffer[type][i].phrase);
					}
				}
			}
		}
        if (output.empty()) {
            output.push_back(L" ");
        }
		return output;
	}

	std::vector<std::wstring> getPersonalIdList() {
		return personal_id_list;
	}
} phrase_container;