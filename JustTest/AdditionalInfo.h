#pragma once

#include <vector>

#include "Collision.h"
#include "UnitInfo.h"

class AdditionalInfo {
	enum Type {
		collision_model,
		unit_info,
		parent_info,
		npc_info,

		INFO_TYPES_COUNT
	};

	std::vector<void *> info_list;
    UnitInfo u_i;

public:

	~AdditionalInfo() {
		for (int i = 0; i < info_list.size(); i++) {
			if (info_list[i]) {
				delete info_list[i];
			}
		}
	}

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
		if (info_list[collision_model]) {
			delete info_list[collision_model];
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
		if (info_list[unit_info]) {
			delete info_list[unit_info];
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
		if (info_list[parent_info]) {
			delete info_list[parent_info];
		}
		info_list[parent_info] = parent;
	}

	void * getNPCInfo() {
		if (info_list.size() > npc_info) {
			return info_list[npc_info];
		}
		return nullptr;
	}

	void setNPCInfo(void * parent) {
		if (info_list.size() <= npc_info) {
			info_list.resize(npc_info + 1);
		}
		if (info_list[npc_info]) {
			delete info_list[npc_info];
		}
		info_list[npc_info] = parent;
	}

	void clear() {
        if (info_list.size() > collision_model && info_list[collision_model]) {
            delete static_cast<CollisionModel *>(info_list[collision_model]);
        }
        if (info_list.size() > unit_info && info_list[unit_info]) {
            delete static_cast<UnitInfo *>(info_list[unit_info]);
        }
		info_list.clear();
	}
};