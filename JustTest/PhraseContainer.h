#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

#include "Settings.h"
#include "ParserInput.h"

struct Phrase {
	std::string phrase, id;
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

		standart_question_npñ,

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

		PHRASE_TYPE_COUNT
	};

private:

	std::vector<std::vector<Phrase>> phrase_buffer;
	std::vector<std::string> phrase_type_strings;

	int is_phrase_type_exists(std::string example) {
		for (int i = 0; i < PHRASE_TYPE_COUNT; i++) {
			if (phrase_type_strings[i] == example) {
				return i;
			}
		}
		return -1;
	}

public:

	PhraseContainer() {
		phrase_type_strings.resize(PHRASE_TYPE_COUNT);
		phrase_buffer.resize(PHRASE_TYPE_COUNT);

		phrase_type_strings[start_description] = "start_description";
		phrase_type_strings[conversation_refuse_npc] = "conversation_refuse_npc";

		phrase_type_strings[greetings_phrase_npc] = "greetings_phrase_npc";

		phrase_type_strings[positive_greetings_answer_player] = "positive_greetings_answer_player";
		phrase_type_strings[neutral_greetings_answer_player] = "neutral_greetings_answer_player";
		phrase_type_strings[negative_greetings_answer_player] = "negative_greetings_answer_player";

		phrase_type_strings[positive_greetings_reaction_npc] = "positive_greetings_reaction_npc";
		phrase_type_strings[neutral_greetings_reaction_npc] = "neutral_greetings_reaction_npc";
		phrase_type_strings[negative_greetings_reaction_npc] = "negative_greetings_reaction_npc";

		phrase_type_strings[standart_question_npñ] = "standart_question_npñ";

		phrase_type_strings[job_question_player] = "job_question_player";
		phrase_type_strings[rumors_question_player] = "rumors_question_player";

		phrase_type_strings[random_joke_player] = "random_joke_player";
		phrase_type_strings[random_positive_joke_reaction_npc] = "random_positive_joke_reaction_npc";
		phrase_type_strings[random_neutral_joke_reaction_npc] = "random_neutral_joke_reaction_npc";
		phrase_type_strings[random_negative_joke_reaction_npc] = "random_negative_joke_reaction_npc";

		phrase_type_strings[rumors_refuse_npc] = "rumors_refuse_npc";

		phrase_type_strings[base_job_refuse_npc] = "base_job_refuse_npc";
		phrase_type_strings[special_job_refuse_npc] = "special_job_refuse_npc";
		phrase_type_strings[special_job_question_player] = "special_job_question_player";
		phrase_type_strings[base_job_question_player] = "base_job_question_player";
		phrase_type_strings[job_refuse_player] = "job_refuse_player";
		phrase_type_strings[job_accept_player] = "job_accept_player";

		phrase_type_strings[farewell_player] = "farewell_player";
		phrase_type_strings[farewell_npc] = "farewell_npc";

		phrase_type_strings[dialog_end_npc] = "dialog_end";
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
		std::ifstream dialog_file;
		dialog_file.open(path);

		std::string input;
		dialog_file >> input;
		if (input != "dialog_file_start") {
			if (settings.isErrorOutputEnabled()) {
				std::cout << "Dialog file is corrupted" << std::endl;
			}
			dialog_file.close();
			return;
		}

		while (true) {
			dialog_file >> input;
			if (input == "dialog_file_end") {
				break;
			}
			if (input != "phrase_start") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Phrase start expected" << std::endl;
				}
				dialog_file.close();
				return;
			}

			Phrase new_phrase;

			dialog_file >> input;
			if (input != "string") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "String marker expected" << std::endl;
				}
				dialog_file.close();
				return;
			}

			while (dialog_file.get() != '\|') {}
			input.clear();
			while (true) {
				unsigned char input_char = dialog_file.get();
				if (input_char == '\|') {
					break;
				}
				if (input_char == '@') {
					input_char = '\n';
				}
				input.push_back(input_char);
			}

			new_phrase.phrase = input;

			dialog_file >> input;
			if (input != "conditions_start") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Conditions marker expected" << std::endl;
				}
				dialog_file.close();
				return;
			}

			while (true) {
				dialog_file >> input;
				if (input == "conditions_end") {
					break;
				}
				if (input == "") {
					if (settings.isErrorOutputEnabled()) {
						std::cout << "Conditions block corrupted" << std::endl;
					}
					dialog_file.close();
					return;
				}
				if (input == "min_politeness") {
					new_phrase.min_politeness = getInt(dialog_file);
				}
				else if (input == "max_politeness") {
					new_phrase.max_politeness = getInt(dialog_file);
				}
				else {
					dialog_file >> input;
				}
				
			}

			dialog_file >> input;
			if (input != "id") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "ID marker expected" << std::endl;
				}
				dialog_file.close();
				return;
			}
			dialog_file >> new_phrase.id;

			dialog_file >> input;
			if (input != "phrase_type") {
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
			}

			dialog_file >> input;
			if (input != "phrase_end") {
				if (settings.isErrorOutputEnabled()) {
					std::cout << "Phrase end expected" << std::endl;
				}
				dialog_file.close();
				return;
			}
		}
	}

	std::vector<std::string> getPhraseBuffer(PhraseType type, int politeness) {
		std::vector<std::string> output;
		if (phrase_buffer.size() >= type) {
			for (int i = 0; i < phrase_buffer[type].size(); i++) {
				if (politeness >= phrase_buffer[type][i].min_politeness && politeness <= phrase_buffer[type][i].max_politeness) {
					output.push_back(phrase_buffer[type][i].phrase);
				}
			}
		}
		return output;
	}
} phrase_container;