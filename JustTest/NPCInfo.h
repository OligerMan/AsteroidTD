#pragma once

#include "PhraseContainer.h"
#include "PlayerRPGProfile.h"

struct DialogInfo {
    struct Answer {
        std::wstring text;
        //PhraseContainer::PhraseType type;
        std::wstring type;
        
        Answer(std::wstring text, std::wstring type) : text(text), type(type) {}
    };
	bool is_player_turn = false;
	std::wstring main_text;
	std::vector<Answer> answers; // if not players turn contains only "Continue"
};

struct DialogInfoModified {
    struct Answer {
        std::wstring text;
        //PhraseContainer::PhraseType type;
        std::wstring type;
    };

    std::wstring main_text;
    std::vector<Answer> answers;
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

    DialogInfo::Answer getContinueAnswer() {
        //std::vector<std::wstring> buffer = phrase_container.getPhraseBuffer(PhraseContainer::continue_phrase_GUI, politeness, personal_id);
        std::vector<std::wstring> buffer = phrase_container.getPhraseBuffer(L"continue_phrase_GUI", politeness, personal_id);
        return DialogInfo::Answer(buffer[rand() * buffer.size() / (RAND_MAX + 1)], L"continue_phrase_GUI");
    }

    DialogInfo::Answer getRerollAnswer() {
        std::vector<std::wstring> buffer = phrase_container.getPhraseBuffer(L"reroll_phrase_GUI", politeness, personal_id);
        return DialogInfo::Answer(buffer[rand() * buffer.size() / (RAND_MAX + 1)], L"reroll_phrase_GUI");
    }

    DialogInfo::Answer getJokeAnswer() {
        std::vector<std::wstring> buffer = phrase_container.getPhraseBuffer(L"joke_phrase_GUI", politeness, personal_id);
        return DialogInfo::Answer(buffer[rand() * buffer.size() / (RAND_MAX + 1)], L"joke_phrase_GUI");
    }

    void setDefaultSettings() {
        politeness = rand() * 200 / (RAND_MAX + 1) - 100;
        prejudices = rand() * 200 / (RAND_MAX + 1) - 100;

        faction = WorldFactionList(rand() % WORLD_FACTIONS_COUNT);

        std::vector<std::wstring> buffer;

        buffer = phrase_container.getPhraseBuffer(L"start_description", politeness, personal_id);
        start_description_string = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
        current_dialog_info.main_text.clear();
        current_dialog_info.main_text = start_description_string;
        current_dialog_info.answers.clear();

        current_dialog_info.answers.push_back(getContinueAnswer());
        current_dialog_info.is_player_turn = false;
    }

public:


	NPCInfo(WorldFactionList faction, std::wstring personal_id) : faction(faction), personal_id(personal_id), base_mission_info(0), special_mission_info(0) {
        setDefaultSettings();
	}

    NPCInfo(WorldFactionList faction) : NPCInfo(faction, L"") {
        std::vector<std::wstring> buffer = phrase_container.getPersonalIdList();
        personal_id = buffer[rand() * buffer.size() / (RAND_MAX + 1)];

        setDefaultSettings();
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
		auto addAnswer = [&](std::wstring phrase) {
			buffer = phrase_container.getPhraseBuffer(phrase, politeness, personal_id);
			current_dialog_info.answers.push_back(DialogInfo::Answer(buffer[rand() * buffer.size() / (RAND_MAX + 1)], phrase));
		};
		auto addMainText = [&](std::wstring phrase) {
			current_dialog_info.main_text.clear();
			buffer = phrase_container.getPhraseBuffer(phrase, politeness, personal_id);
			current_dialog_info.main_text = buffer[rand() * buffer.size() / (RAND_MAX + 1)];
		};
		auto backToStandartQuestion = [&]() {
			current_stage = standart_question;
			addMainText(L"standart_question_npc");
			current_dialog_info.answers.clear();
			if ((jobAvailable() || specialJobAvailable())) {
				addAnswer(L"job_question_player");
			}
			if (rumorsAvaliable()) {
				addAnswer(L"rumors_question_player");
			}
			current_dialog_info.answers.push_back(getJokeAnswer());
			addAnswer(L"farewell_player");
			current_dialog_info.is_player_turn = false;
		};
		auto goEnd = [&]() {
			current_stage = dialog_end;
			addMainText(L"dialog_end_npc");
			current_dialog_info.answers.clear();
			current_dialog_info.answers.push_back(getContinueAnswer());
		};

		int rand_coef = 0, joke_mark = 0;
		switch (current_stage) {
		case dialog_start:
			if (prejudices + rpg_profile.getFactionFame(faction) + rpg_profile.getGlobalFame() < -150) {
				current_stage = conversation_refuse;
				addMainText(L"conversation_refuse_npc");
				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueAnswer());
				current_dialog_info.is_player_turn = false;
			}
			else {
				current_stage = greetings_phrase;
				addMainText(L"greetings_phrase_npc");
				current_dialog_info.answers.clear();
				addAnswer(L"positive_greetings_answer_player");
				addAnswer(L"neutral_greetings_answer_player");
				addAnswer(L"negative_greetings_answer_player");
				addAnswer(L"farewell_player");
				current_dialog_info.answers.push_back(getRerollAnswer());
				current_dialog_info.is_player_turn = false;
			}
			break;
		case conversation_refuse:
			goEnd();
			current_dialog_info.is_player_turn = false;
			break;
		case greetings_phrase:
			if (current_dialog_info.answers[answer_num].type == L"reroll_phrase_GUI") {
				current_dialog_info.answers.clear();
				addAnswer(L"positive_greetings_answer_player");
				addAnswer(L"neutral_greetings_answer_player");
				addAnswer(L"negative_greetings_answer_player");
				addAnswer(L"farewell_player");
				current_dialog_info.answers.push_back(getRerollAnswer());
				current_dialog_info.is_player_turn = false;
			}
			if (current_dialog_info.answers[answer_num].type == L"farewell_player") {
				goEnd();
			}
			if (current_dialog_info.answers[answer_num].type == L"negative_greetings_answer_player") {
				current_stage = negative_greeting_reaction;
				addMainText(L"negative_greetings_reaction_npc");
				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueAnswer());
				current_dialog_info.is_player_turn = true;
			}
			if (current_dialog_info.answers[answer_num].type == L"neutral_greetings_answer_player") {
				current_stage = neutral_greeting_reaction;
				addMainText(L"neutral_greetings_reaction_npc");
				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueAnswer());
				current_dialog_info.is_player_turn = true;
			}
			if (current_dialog_info.answers[answer_num].type == L"positive_greetings_answer_player") {
				current_stage = positive_greeting_reaction;
				addMainText(L"positive_greetings_reaction_npc");
				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueAnswer());
				current_dialog_info.is_player_turn = true;
			}
			break;
		case negative_greeting_reaction:
			base_job_available = true;
			special_job_available = false;
			rand_coef = rand() % 10;
			if (rand_coef == 0) {
				current_stage = conversation_refuse;
				addMainText(L"conversation_refuse_npc");
				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueAnswer());
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
			if (current_dialog_info.answers[answer_num].type == L"job_question_player") {
				if (!jobAvailable()) {
					current_stage = job_refuse;

					addMainText(L"base_job_refuse_npc");

					current_dialog_info.answers.clear();
					current_dialog_info.answers.push_back(getContinueAnswer());
					current_dialog_info.is_player_turn = false;
				}
				else {
					if (!job_question_passed && jobAvailable()) {
						current_stage = job_selection;

						current_dialog_info.main_text = base_mission_info.getShortDescription();

						current_dialog_info.answers.clear();
						addAnswer(L"job_accept_player"); 
						if (!special_job_question_passed) {
							addAnswer(L"special_job_question_player");
						}
						addAnswer(L"job_refuse_player");
					}
					else {
						current_stage = special_job_offer;

						current_dialog_info.main_text = special_mission_info.getBroadDescription();


						current_dialog_info.answers.clear();
						addAnswer(L"job_accept_player");
						addAnswer(L"job_refuse_player");
					}
					current_dialog_info.is_player_turn = false;
				}
			}
			else if (current_dialog_info.answers[answer_num].type == L"rumors_question_player") {
				if (prejudices + rpg_profile.getFactionFame(faction) + rpg_profile.getGlobalFame() < 0) {
					current_stage = rumors_refuse;
					addMainText(L"rumors_refuse_npc");
					current_dialog_info.answers.clear();
					current_dialog_info.answers.push_back(getContinueAnswer());
					current_dialog_info.is_player_turn = false;
				}
				else {
					current_stage = rumors_refuse;
					addMainText(L"rumors_refuse_npc");
					current_dialog_info.answers.clear();
					current_dialog_info.answers.push_back(DialogInfo::Answer(L"<TODO: rumors generator>", L""));
					current_dialog_info.is_player_turn = false;
				}
			}
			else if (current_dialog_info.answers[answer_num].type == L"joke_phrase_GUI") {
				current_stage = joke;

				addMainText(L"random_joke_player");

				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueAnswer());
				current_dialog_info.is_player_turn = false;
			}
			else if (current_dialog_info.answers[answer_num].type == L"farewell_player") {
				current_stage = farewell;

				addMainText(L"farewell_npc");

				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueAnswer());
				current_dialog_info.is_player_turn = false;
			}
			break;
		case joke:
			joke_mark = rand() * 100 / (RAND_MAX + 1);
			if (joke_mark <= 33) {
				current_stage = negative_joke_reaction;

				addMainText(L"random_negative_joke_reaction_npc");

				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueAnswer());
				current_dialog_info.is_player_turn = false;
			}
			else if (joke_mark <= 66) {
				current_stage = neutral_joke_reaction;

				addMainText(L"random_neutral_joke_reaction_npc");

				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueAnswer());
				current_dialog_info.is_player_turn = false;
			}
			else {
				current_stage = positive_joke_reaction;

				addMainText(L"random_positive_joke_reaction_npc");

				current_dialog_info.answers.clear();
				current_dialog_info.answers.push_back(getContinueAnswer());
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
			if (current_dialog_info.answers[answer_num].type == L"job_accept_player") {
				current_stage = standart_job;

				current_dialog_info.main_text = base_mission_info.getBroadDescription();

				current_dialog_info.answers.clear();
				addAnswer(L"job_accept_player");
				addAnswer(L"job_refuse_player");
				current_dialog_info.is_player_turn = false;
			}
			else if (current_dialog_info.answers[answer_num].type == L"special_job_question_player") {

				if (special_job_available || prejudices + rpg_profile.getFactionFame(faction) + rpg_profile.getGlobalFame() > 150) {
					current_stage = special_job_offer;

					current_dialog_info.main_text = special_mission_info.getShortDescription();

					current_dialog_info.answers.clear();
					addAnswer(L"job_accept_player");
					addAnswer(L"job_refuse_player");
				}
				else {
					current_stage = job_refuse;
					addMainText(L"special_job_refuse_npc");
					current_dialog_info.answers.clear();
					current_dialog_info.answers.push_back(getContinueAnswer());
				}
				
				current_dialog_info.is_player_turn = false;
			}
			else if (current_dialog_info.answers[answer_num].type == L"job_refuse_player") {
				backToStandartQuestion();
			}
			break;
		case special_job_offer:
			if (current_dialog_info.answers[answer_num].type == L"job_accept_player") {
				current_stage = special_job;

				current_dialog_info.main_text = special_mission_info.getBroadDescription();

				current_dialog_info.answers.clear();
				addAnswer(L"job_accept_player");
				addAnswer(L"job_refuse_player");
				current_dialog_info.is_player_turn = false;
			}
			else if (current_dialog_info.answers[answer_num].type == L"job_refuse_player") {
				backToStandartQuestion();
			}
			break;
		case standart_job:
			job_question_passed = true;
			if (current_dialog_info.answers[answer_num].type == L"job_accept_player") {
				rpg_profile.addMission(base_mission_info);

				backToStandartQuestion();
			}
			else if (current_dialog_info.answers[answer_num].type == L"job_refuse_player") {
				backToStandartQuestion();
			}
			break;
		case special_job:
			special_job_question_passed = true;
			if (current_dialog_info.answers[answer_num].type == L"job_accept_player") {
				rpg_profile.addMission(special_mission_info);

				backToStandartQuestion();
			}
			else if (current_dialog_info.answers[answer_num].type == L"job_refuse_player") {
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
			current_dialog_info.answers.push_back(getContinueAnswer());
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