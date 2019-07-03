#pragma once

#include <vector>

#include "Object.h"
#include "EventBuffer.h"
#include "GUIParser.h"


class GUIManager {

	std::vector<std::vector<Object *>> gui_elements;

	EventBuffer gui_event_buffer;
	EventBuffer external_event_buffer;

	Object * last_clicked_object = nullptr;
	Object * selected_object = nullptr;

	bool checkClick(Point click) {
		Object * cursor = new Object(click);

		if (!gui_elements.empty()) {
			for (int layer = (int)gui_elements.size() - 1; layer >= 0; layer--) {
				for (int i = 0; i < gui_elements[layer].size(); i++) {
					
					Point pos = gui_elements[layer][i]->getPosition();
					if (layer == 1 && (pos - click).getLength() <= 50) {
						if (gui_elements[layer][i] != last_clicked_object) {

							gui_event_buffer.addEvent(clicked, gui_elements[layer][i], nullptr);
							std::cout << "Click" << std::endl;

							last_clicked_object = gui_elements[layer][i];
						}
						else {
							gui_event_buffer.addEvent(pressed, gui_elements[layer][i], nullptr);
							std::cout << "Press" << std::endl;
						}
						delete cursor;
						return true;
					}
					else if (last_clicked_object == gui_elements[layer][i]) {
						gui_event_buffer.addEvent(released, gui_elements[layer][i], nullptr);
						std::cout << "Release" << std::endl;

						last_clicked_object = nullptr;
					}
				}
			}
		}
		delete cursor;
		return false;
	}

	void processEventBuffer() {
		Event cur_event = gui_event_buffer.getEvent();
		if (cur_event.getEventType() != null_event) {
			if (cur_event.getEventType() == clicked && cur_event.getFirstObject()->getObjectType() != redactor_back) {
				if (selected_object) {
					delete selected_object;
				}
				selected_object = new Object(*cur_event.getFirstObject());
				selected_object->setAutoOrigin();
			}
		}
		
	}

public:

	GUIManager() {}

	GUIManager(std::vector<std::vector<Object*>> * redactor_mode_objects) {
	}


	bool loadFromFile(std::string path) {
		gui_parser(path, &gui_elements);
	}

	bool processFrame(Point click, Point viewport_pos) {
		bool output = checkClick(click);
		processEventBuffer();

		return output;
	}

	Event getEvent() {
		return external_event_buffer.getEvent();
	}

	std::vector<std::vector<Object *>> * getObjectsBuffer() {
		return &gui_elements;
	}

	Object * getSelectedObject() {
		return selected_object;
	}
};