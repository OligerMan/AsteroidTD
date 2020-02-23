#pragma once

#include "PhraseContainer.h"
#include "PlayerRPGProfile.h"

struct DialogInfo {
	bool is_player_turn = false;
	std::wstring main_text;
	std::vector<std::wstring> answers; // if not players turn contains only "Continue"
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
	DialogInfo current_dialog_info;
	WorldFactionList faction = WORLD_FACTIONS_COUNT;

    Mission base_mission_info;
    Mission special_mission_info;

	std::wstring start_description_string;
	std::wstring personal_id;

	// personal qualities(from -100 to 100)

	int politeness = 0;
	int prejudices = 0;

    std::wstring getContinueString() {
        std::vector<std::wstring> buffer = phrase_container.getPhraseBuffer(PhraseContainer::continue_phrase_GUI, politeness, personal_id);
        return buffer[rand() * buffer.size() / (RAND_MAX + 1)];
    }

    std::wstring getRerollString() {
        std::vector<std::wstring> buffer = phrase_container.getPhraseBuffer(PhraseContainer::reroll_phrase_GUI, politeness, personal_id);
        return buffer[rand() * buffer.size() / (RAND_MAX + 1)];
    }

    std::wstring getJokeString() {
        std::vector<std::wstring> buffer = phrase_container.getPhraseBuffer(PhraseContainer::joke_phrase_GUI, politeness, personal_id);
        return buffer[rand() * buffer.size() / (RAND_MAX + 1)];
    }

public:

	/*NPCInfo(WorldFactionList faction) : faction(faction) {
		politeness = rand() * 200 / (RAND_MAX + 1) - 100;
		prejudices = rand() * 200 / (RAND_MAX + 1) - 100;

		faction = WorldFactionList(rand() % WORLD_FACTIONS_COUNT);

		std::vector<std::wstring> buffer;

		buffer = phrase_container.getPersonalIdList();
		personal_id = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

		buffer = phrase_container.getPhraseBuffer(PhraseContainer::start_description, politeness, personal_id);
		start_description_string = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
		current_dialog_info.main_text.clear();
		current_dialog_info.main_text = start_description_string;
		current_dialog_info.answers.clear();
        
		current_dialog_info.answers.push_back(getContinueString());
		current_dialog_info.is_player_turn = false;
	}*/

	NPCInfo(WorldFactionList faction, std::wstring personal_id) : faction(faction), personal_id(personal_id), base_mission_info(0), special_mission_info(0) {
		politeness = rand() * 200 / (RAND_MAX + 1) - 100;
		prejudices = rand() * 200 / (RAND_MAX + 1) - 100;

		faction = WorldFactionList(rand() % WORLD_FACTIONS_COUNT);

		std::vector<std::wstring> buffer;

		buffer = phrase_container.getPhraseBuffer(PhraseContainer::start_description, politeness, personal_id);
		start_description_string = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
		current_dialog_info.main_text.clear();
		current_dialog_info.main_text = start_description_string;
		current_dialog_info.answers.clear();

		current_dialog_info.answers.push_back(getContinueString());
		current_dialog_info.is_player_turn = false;
	}

    NPCInfo(WorldFactionList faction) : NPCInfo(faction, L"") {
        std::vector<std::wstring> buffer = phrase_container.getPersonalIdList();
        personal_id = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
    }

	ConversationStage getCurrentStage() {
		return current_stage;
	}

	void nextTurn(int answer_num) {
		if (answer_num < 0 || answer_num >= current_dialog_info.answers.size()) {
			return;
		}
		std::vector<std::wstring> buffer;

		auto jobAvailable = [&]() {
			return (!job_question_passed && base_job_available && base_mission_info.getMissionStageType() != MissionStage::TYPE_COUNT);
		};
		auto specialJobAvailable = [&]() {
			return (!special_job_question_passed && special_job_available && special_mission_info.getMissionStageType() != MissionStage::TYPE_COUNT);
		};
		auto rumorsAvaliable = [&]() {
			return (!rumors_question_passed && rumors_available);
		};
		auto addAnswer = [&](PhraseContainer::PhraseType phrase) {
			buffer = phrase_container.getPhraseBuffer(phrase, politeness, personal_id);
			current_dialog_info.answers.push_back(buffer[rand() * buffer.size() / (RAND_MAX + 1)]);
		};
		auto addMainText = [&](PhraseContainer::PhraseType phrase) {
			current_dialog_info.main_text.clear();
			buffer = phrase_container.getPhraseBuffer(phrase, politeness, personal_id);
			current_dialog_info.main_text = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
		};
		auto backToStandartQuestion = [&]() {
			current_stage = standart_question;
			addMainText(PhraseContainer::standart_question_npc);
			current_dialog_info.answers.clear();
			if ((jobAvailable() || specialJobAvailable())) {
				addAnswer(PhraseContainer::job_question_player);
			}
			if (rumorsAvaliable()) {
				addAnswer(PhraseContainer::rumors_question_player);
			}
			current_dialog_info.answers.push_back(getJokeString());
			addAnswer(PhraseContainer::farewell_player);
			current_dialog_info.is_player_turn = false;
		};
		auto goEnd = [&]() {
			current_stage = dialog_end;
			addMainText(PhraseContainer::dialog_end_npc);
			current_dialog_info.answers.clear();
			current_dialog_info.answers.push_back(getContinueString());
		};

		int rand_coef = 0, joke_mark = 0;
		switch (current_stage) {
		case dialog_start:
			if (prejudices + rpg_profile.getFactionFame(faction) + rpg_profile.getGlobalFame() < -150) {
				current_stage = conversation_refuse;
				addMainText(PhraseContainer::conversation_refuse_npc);
				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueString());
				current_dialog_info.is_player_turn = false;
			}
			else {
				current_stage = greetings_phrase;
				addMainText(PhraseContainer::greetings_phrase_npc);
				current_dialog_info.answers.clear();
				addAnswer(PhraseContainer::positive_greetings_answer_player);
				addAnswer(PhraseContainer::neutral_greetings_answer_player);
				addAnswer(PhraseContainer::negative_greetings_answer_player);
				addAnswer(PhraseContainer::farewell_player);
				current_dialog_info.answers.push_back(getRerollString());
				current_dialog_info.is_player_turn = false;
			}
			break;
		case conversation_refuse:
			goEnd();
			current_dialog_info.is_player_turn = false;
			break;
		case greetings_phrase:
			if (answer_num == 4) {
				current_dialog_info.answers.clear();
				addAnswer(PhraseContainer::positive_greetings_answer_player);
				addAnswer(PhraseContainer::neutral_greetings_answer_player);
				addAnswer(PhraseContainer::negative_greetings_answer_player);
				addAnswer(PhraseContainer::farewell_player);
				current_dialog_info.answers.push_back(getRerollString());
				current_dialog_info.is_player_turn = false;
			}
			if (answer_num == 3) {
				goEnd();
			}
			if (answer_num == 2) {
				current_stage = negative_greeting_reaction;
				addMainText(PhraseContainer::negative_greetings_reaction_npc);
				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueString());
				current_dialog_info.is_player_turn = true;
			}
			if (answer_num == 1) {
				current_stage = neutral_greeting_reaction;
				addMainText(PhraseContainer::neutral_greetings_reaction_npc);
				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueString());
				current_dialog_info.is_player_turn = true;
			}
			if (answer_num == 0) {
				current_stage = positive_greeting_reaction;
				addMainText(PhraseContainer::positive_greetings_reaction_npc);
				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueString());
				current_dialog_info.is_player_turn = true;
			}
			break;
		case negative_greeting_reaction:
			base_job_available = true;
			special_job_available = false;
			rand_coef = rand() % 10;
			if (rand_coef == 0) {
				current_stage = conversation_refuse;
				addMainText(PhraseContainer::conversation_refuse_npc);
				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueString());
				current_dialog_info.is_player_turn = false;
			}
			else {
				backToStandartQuestion();
			}
			
			break;
		case neutral_greeting_reaction:
			base_job_available = true;
			special_job_available = true;

			backToStandartQuestion();
		case positive_greeting_reaction:
			base_job_available = true;
			special_job_available = true;
			rumors_available = true;

			backToStandartQuestion();
			break;
		case standart_question:
			if (answer_num == 0 && (jobAvailable() || specialJobAvailable())) {
				if (!jobAvailable()) {
					current_stage = job_refuse;

					addMainText(PhraseContainer::base_job_refuse_npc);

					current_dialog_info.answers.clear();
					current_dialog_info.answers.push_back(getContinueString());
					current_dialog_info.is_player_turn = false;
				}
				else {
					if (!job_question_passed && jobAvailable()) {
						current_stage = job_selection;

						current_dialog_info.main_text = base_mission_info.getShortDescription();

						current_dialog_info.answers.clear();
						addAnswer(PhraseContainer::job_accept_player); 
						if (!special_job_question_passed) {
							addAnswer(PhraseContainer::special_job_question_player);
						}
						addAnswer(PhraseContainer::job_refuse_player);
					}
					else {
						current_stage = special_job_offer;

						current_dialog_info.main_text = special_mission_info.getBroadDescription();


						current_dialog_info.answers.clear();
						addAnswer(PhraseContainer::job_accept_player);
						addAnswer(PhraseContainer::job_refuse_player);
					}
					current_dialog_info.is_player_turn = false;
				}
			}
			else if ((answer_num == 1 && (jobAvailable() || specialJobAvailable()) && rumorsAvaliable()) || answer_num == 0) {
				if (prejudices + rpg_profile.getFactionFame(faction) + rpg_profile.getGlobalFame() < 0) {
					current_stage = rumors_refuse;
					addMainText(PhraseContainer::rumors_refuse_npc);
					current_dialog_info.answers.clear();
					current_dialog_info.answers.push_back(getContinueString());
					current_dialog_info.is_player_turn = false;
				}
				else {
					current_stage = rumors_refuse;
					addMainText(PhraseContainer::rumors_refuse_npc);
					current_dialog_info.answers.clear();
					current_dialog_info.answers.push_back(L"<TODO: rumors generator>");
					current_dialog_info.is_player_turn = false;
				}
			}
			else if ((answer_num == 2 && (jobAvailable() || specialJobAvailable()) && rumorsAvaliable()) || (answer_num == 1 && ((jobAvailable() || specialJobAvailable()) ^ rumorsAvaliable())) || answer_num == 0) {
				current_stage = joke;

				addMainText(PhraseContainer::random_joke_player);

				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueString());
				current_dialog_info.is_player_turn = false;
			}
			else if ((answer_num == 3 && (jobAvailable() || specialJobAvailable()) && rumorsAvaliable()) || (answer_num == 2 && ((jobAvailable() || specialJobAvailable()) ^ rumorsAvaliable())) || answer_num == 1) {
				current_stage = farewell;

				addMainText(PhraseContainer::farewell_npc);

				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueString());
				current_dialog_info.is_player_turn = false;
			}
			break;
		case joke:
			joke_mark = rand() * 100 / (RAND_MAX + 1);
			if (joke_mark <= 33) {
				current_stage = negative_joke_reaction;

				addMainText(PhraseContainer::random_negative_joke_reaction_npc);

				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueString());
				current_dialog_info.is_player_turn = false;
			}
			else if (joke_mark <= 66) {
				current_stage = neutral_joke_reaction;

				addMainText(PhraseContainer::random_neutral_joke_reaction_npc);

				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueString());
				current_dialog_info.is_player_turn = false;
			}
			else {
				current_stage = positive_joke_reaction;

				addMainText(PhraseContainer::random_positive_joke_reaction_npc);

				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueString());
				current_dialog_info.is_player_turn = false;
			}
			break;
		case negative_joke_reaction:
			backToStandartQuestion();
			break;
		case neutral_joke_reaction:
			backToStandartQuestion();
			break;
		case positive_joke_reaction:
			backToStandartQuestion();
			break;
		case job_selection:
			if (answer_num == 0) {
				current_stage = standart_job;

				current_dialog_info.main_text = base_mission_info.getBroadDescription();

				current_dialog_info.answers.clear();
				addAnswer(PhraseContainer::job_accept_player);
				addAnswer(PhraseContainer::job_refuse_player);
				current_dialog_info.is_player_turn = false;
			}
			else if (answer_num == 1 && !special_job_question_passed) {

				if (special_job_available || prejudices + rpg_profile.getFactionFame(faction) + rpg_profile.getGlobalFame() > 150) {
					current_stage = special_job_offer;

					current_dialog_info.main_text = special_mission_info.getShortDescription();

					current_dialog_info.answers.clear();
					addAnswer(PhraseContainer::job_accept_player);
					addAnswer(PhraseContainer::job_refuse_player);
				}
				else {
					current_stage = job_refuse;
					addMainText(PhraseContainer::special_job_refuse_npc);
					current_dialog_info.answers.clear();
					current_dialog_info.answers.push_back(getContinueString());
				}
				
				current_dialog_info.is_player_turn = false;
			}
			else {
				backToStandartQuestion();
			}
			break;
		case special_job_offer:
			if (answer_num == 0) {
				current_stage = special_job;

				current_dialog_info.main_text = special_mission_info.getBroadDescription();

				current_dialog_info.answers.clear();
				addAnswer(PhraseContainer::job_accept_player);
				addAnswer(PhraseContainer::job_refuse_player);
				current_dialog_info.is_player_turn = false;
			}
			else {
				backToStandartQuestion();
			}
			break;
		case standart_job:
			job_question_passed = true;
			if (answer_num == 0) {
				rpg_profile.addMission(base_mission_info);

				backToStandartQuestion();
			}
			else {
				backToStandartQuestion();
			}
			break;
		case special_job:
			special_job_question_passed = true;
			if (answer_num == 0) {
				rpg_profile.addMission(special_mission_info);

				backToStandartQuestion();
			}
			else {
				backToStandartQuestion();
			}
			break;
		case job_refuse:
			backToStandartQuestion();
			break;
		case rumors_refuse:
			backToStandartQuestion();
			break;
		case farewell:
			goEnd();
			break;
		case dialog_end:
			special_job_available = false;
			base_job_available = true;
			current_stage = dialog_start;
			current_dialog_info.main_text = start_description_string;
			current_dialog_info.answers.clear();
			current_dialog_info.answers.push_back(getContinueString());
			current_dialog_info.is_player_turn = false;
			break;
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