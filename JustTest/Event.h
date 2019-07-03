#pragma once

#include "Object.h"
#include "EventTypes.h"

class Event {
	Object * obj1 = nullptr, * obj2 = nullptr;
	EventType event_type = null_event;

public:

	Event(Object * obj1, Object * obj2, EventType event_type) : obj1(obj1), obj2(obj2), event_type(event_type) {}

	Object * getFirstObject() {
		return obj1;
	}

	Object * getSecondObject() {
		return obj2;
	}

	EventType getEventType() {
		return event_type;
	}
};