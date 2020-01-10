#pragma once

#include <vector>

#include "Collision.h"
#include "UnitInfo.h"
#include "Object.h"

class AdditionalInfo {
	enum Type {
		collision_model,
		unit_info,
		parent_info,

		INFO_TYPES_COUNT
	};

	std::vector<void *> info_list;

public:

	CollisionModel * getCollisionModel() {
		if (info_list.size() > collision_model) {
			return static_cast<CollisionModel *>(info_list[collision_model]);
		}
		return nullptr;
	}

	void setCollisionModel(void * new_col_model) {
		if (info_list.size() <= collision_model) {
			info_list.resize(collision_model + 1);
		}
		info_list[collision_model] = new_col_model;
	}

	UnitInfo * getUnitInfo() {
		if (info_list.size() > unit_info) {
			return static_cast<UnitInfo *>(info_list[unit_info]);
		}
		return nullptr;
	}

	void setUnitInfo(void * new_unit_info) {
		if (info_list.size() <= unit_info) {
			info_list.resize(unit_info + 1);
		}
		info_list[unit_info] = new_unit_info;
	}

	void * getParent() {
		if (info_list.size() > parent_info) {
			return info_list[parent_info];
		}
		return nullptr;
	}

	void setParent(void * parent) {
		if (info_list.size() <= parent_info) {
			info_list.resize(parent_info + 1);
		}
		info_list[parent_info] = parent;
	}

	void clear() {
		info_list.clear();
	}
};