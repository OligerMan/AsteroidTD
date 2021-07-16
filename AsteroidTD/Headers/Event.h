#pragma once

#include "Object.h"
#include "EventTypes.h"

class Event {
	std::vector<void *> container;
	EventType event_type = null_event;

public:

	Event(void * obj1, void * obj2, EventType event_type) : event_type(event_type) {
		container.push_back(obj1);
		container.push_back(obj2);
	}

	Event(void * obj, EventType event_type) : event_type(event_type) {
		container.push_back(obj);
	}

	Event(EventType event_type) : event_type(event_type) {}

	void * getData(unsigned int data_num) {
		if (data_num < container.size()) {
			return container[data_num];
		}
		return nullptr;
	}

	void addData(void * data) {
		container.push_back(data);
	}

	EventType getEventType() {
		return event_type;
	}
};