#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <codecvt>
#include <locale>
#include <map>

#include "../Utils/Settings.h"
#include "../Addons/ParserInput.h"
#include "../Utils/FileSearch.h"

struct Phrase {
    struct Conditions {
        std::wstring name, value;
    };
    std::wstring phrase, id;
	int min_politeness = -100;
	int max_politeness = 100;

    std::vector<Conditions> conditions;
};

class PhraseContainer {
public:
	

private:

    std::map<std::wstring, std::vector<Phrase>> new_phrase_buffer;
	std::vector<std::wstring> personal_id_list;


	int is_personal_id_exists(std::wstring example) {
		for (int i = 0; i < personal_id_list.size(); i++) {
			if (personal_id_list[i] == example) {
				return i;
			}
		}
		return -1;
	}

public:

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
            auto phrase_iter = new_phrase_buffer.find(input);
            if (phrase_iter == new_phrase_buffer.end()) {
                new_phrase_buffer.insert({ input, std::vector<Phrase>() });
                phrase_iter = new_phrase_buffer.find(input);
            }
            phrase_iter->second.push_back(new_phrase);
            if (is_personal_id_exists(new_phrase.id) == -1) {
                personal_id_list.push_back(new_phrase.id);
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
            parseFromFile(path + path_separator + *i);
        }
    }

    std::vector<std::wstring> getPhraseBuffer(std::wstring type, int politeness = 0, std::wstring id = L"") {
        std::vector<std::wstring> output;
        bool id_spec_phrase = false;
        auto iter = new_phrase_buffer.find(type);
        if (iter != new_phrase_buffer.end()) {
            for (int i = 0; i < iter->second.size(); i++) {
                if (politeness >= iter->second[i].min_politeness && politeness <= iter->second[i].max_politeness) {
                    if (id.empty() || id == L"common" || id == iter->second[i].id || iter->second[i].id == L"common") {
                        output.push_back(iter->second[i].phrase);
                        if (id == iter->second[i].id) {
                            id_spec_phrase = true;
                            break;
                        }
                    }
                }
            }
            if (id_spec_phrase) {
                output.clear();
                for (int i = 0; i < iter->second.size(); i++) {
                    if (politeness >= iter->second[i].min_politeness && politeness <= iter->second[i].max_politeness) {
                        if (id == iter->second[i].id) {
                            output.push_back(iter->second[i].phrase);
                        }
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
