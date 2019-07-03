#pragma once

#include <vector>

#include "Event.h"

class EventBuffer {

	std::vector<Event> event_buffer;

public:

	void addEvent(Object * object1, Object * object2) {
		EventType event_type = getEventType(object1->getObjectCollisionType(), object2->getObjectCollisionType());
		event_buffer.push_back(Event(object1, object2, event_type));
	}

	void addEvent(EventType event_type, Object * object1, Object * object2) {
		event_buffer.push_back(Event(object1, object2, event_type));
	}

	Event getEvent() {
		if (event_buffer.size() != 0) {
			Event output = event_buffer[event_buffer.size() - 1];
			event_buffer.pop_back();
			return output;
		}
		else {
			return Event(nullptr, nullptr, null_event);
		}
	}

};