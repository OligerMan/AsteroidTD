#pragma once

#include "ObjectTypes.h"
#include "VisualInfo.h"
#include "AdditionalInfo.h"

class Object {


	CollisionType object_col_type = null_col;
	VisualInfo vis_info;
	ObjectType object_type_info = null;
	AdditionalInfo add_info;
    Object * die_object = nullptr;   // object created after desruction of main object. Allows to perform chain transformation of objects

	bool deleted = false;
	bool discovered = false;
	std::vector<Object *> attached_objects;
	int time_left = -1;
    bool attachment_fixed = true;
    float drone_factory_cooldown = 0;

public:

	~Object() {
        if (die_object) {
            die_object->setPosition(this->getPosition());
        }
		if (add_info.getCollisionModel()) {
			//delete add_info.getCollisionModel();
		}
		if (add_info.getUnitInfo()) {
			//delete add_info.getUnitInfo();
		}
		for (int i = 0; i < attached_objects.size(); i++) {
			attached_objects[i]->deleteObject();
		}
		attached_objects.clear();
		add_info.clear();
	}

	Object() {}

	Object(
		Point point
	) {
		add_info.setCollisionModel(new CollisionModel());
		add_info.getCollisionModel()->setPosition(point);
		add_info.getCollisionModel()->addCircle(Point(), 0);
		time_left = lifetime[object_type_info];
	}

	Object(
		Point point,
		Point origin
	) : Object(
		point
	) {
		add_info.getCollisionModel()->setOrigin(origin);
		time_left = lifetime[object_type_info];
	}

	Object(
		Point point,
		CollisionType obj_col_type,
		VisualInfo visual_info
	) : Object(
		point
	) {
		object_col_type = obj_col_type;
		vis_info = visual_info;
		time_left = lifetime[object_type_info];
	}

	Object(
		Point point,
		Point origin,
		CollisionType obj_col_type,
		VisualInfo visual_info
	) : Object(
		point,
		origin
	) {
		object_col_type = obj_col_type;
		vis_info = visual_info;
		time_left = lifetime[object_type_info];
	}

	Object(
		Point point,
		Point origin,
		ObjectType new_object_type,
		CollisionType obj_col_type,
		VisualInfo visual_info
	) : Object(
		point,
		origin,
		obj_col_type,
		visual_info
	) {
		object_type_info = new_object_type;

		add_info.setCollisionModel(new CollisionModel(collision_info_set[obj_col_type]));
		add_info.getCollisionModel()->setPosition(point);
		add_info.getCollisionModel()->setOrigin(origin);

		add_info.setUnitInfo(new UnitInfo(unit_info_set[new_object_type]));

		time_left = lifetime[object_type_info];
	}

	Object(
		Point point,
		Point origin,
		ObjectType new_object_type,
		CollisionType obj_col_type,
		VisualInfo visual_info,
		float hp,
		float mana,
		float endurance
	) : Object(
		point,
		origin,
		new_object_type,
		obj_col_type,
		visual_info
	) {
		add_info.getUnitInfo()->setHealth(hp);
		add_info.getUnitInfo()->setMana(mana);
		add_info.getUnitInfo()->setEndurance(endurance);
		time_left = lifetime[object_type_info];
	}

	CollisionModel * getCollisionModel() {
		return add_info.getCollisionModel();
	}

	CollisionType getObjectCollisionType() {
		return object_col_type;
	}

	SpriteType getObjectSpriteType() {
		return vis_info.object_spr_type;
	}

	AnimationType getObjectAnimationType() {
		return vis_info.animation_type;
	}

	int getFrameNumber() {
		return vis_info.frame_num / vis_info.frame_duration;
	}

	int getFrameDuration() {
		return vis_info.frame_duration;
	}

	void frameIncrement() {
		vis_info.frame_num++;
	}

	Point getSpeed() {
		if (isDeleted() || !add_info.getCollisionModel()) {
			return Point();
		}
		return add_info.getCollisionModel()->getSpeed();
	}

	void setSpeed(Point speed) {
		if (isDeleted() || !add_info.getCollisionModel()) {
			return;
		}
		add_info.getCollisionModel()->setSpeed(speed);
	}

	void changeSpeed(Point difference) {
		if (isDeleted() || !add_info.getCollisionModel()) {
			return;
		}
		add_info.getCollisionModel()->changeSpeed(difference);
	}

	Point getPosition() {
		if (!add_info.getCollisionModel()) {
			return Point();
		}
		return add_info.getCollisionModel()->getPosition();
	}

	void setPosition(Point origin) {
		if (isDeleted() || !add_info.getCollisionModel()) {
			return;
		}
		for (int i = 0; i < attached_objects.size(); i++) {
			if (attached_objects[i]->getObjectType() != bullet) {
				attached_objects[i]->setPosition(origin + (getPosition() - attached_objects[i]->getPosition()));
			}
		}
		add_info.getCollisionModel()->setPosition(origin);
	}

	void changePosition(Point difference) {
		if (isDeleted() || !add_info.getCollisionModel()) {
			return;
		}
		for (int i = 0; i < attached_objects.size(); i++) {
			if (attached_objects[i]->getObjectType() != bullet) {
				attached_objects[i]->changePosition(difference);
			}
		}
		add_info.getCollisionModel()->changePosition(difference);
	}

	void forceChangePosition(Point difference) {
		if (isDeleted() || !add_info.getCollisionModel()) {
			return;
		}
		for (int i = 0; i < attached_objects.size(); i++) {
			if (attached_objects[i]->getObjectType() != bullet) {
				attached_objects[i]->forceChangePosition(difference);
			}
		}
		add_info.getCollisionModel()->forceChangePosition(difference);
	}

	Point getOrigin() {
		if (isDeleted() || !add_info.getCollisionModel()) {
			return Point();
		}
		return add_info.getCollisionModel()->getOrigin();
	}

	void setOrigin(Point origin) {
		if (isDeleted() || !add_info.getCollisionModel()) {
			return;
		}
		add_info.getCollisionModel()->setOrigin(origin);
	}

	void changeOrigin(Point difference) {
		if (isDeleted() || !add_info.getCollisionModel()) {
			return;
		}
		add_info.getCollisionModel()->changeOrigin(difference);
	}

	void setAutoOrigin() {
		if (isDeleted() || !add_info.getCollisionModel()) {
			return;
		}
		add_info.getCollisionModel()->setAutoOrigin();
	}

	double getAngle() {
		if (isDeleted() || !add_info.getCollisionModel()) {
			return 0;
		}
		return add_info.getCollisionModel()->getAngle();
	}

	void setAngle(double angle) {
		if (isDeleted() || !add_info.getCollisionModel()) {
			return;
		}
		add_info.getCollisionModel()->setAngle(angle);
	}

	void changeAngle(double difference) {
		if (isDeleted() || !add_info.getCollisionModel()) {
			return;
		}
		for (int i = 0; i < attached_objects.size(); i++) {
			if (attached_objects[i]->getObjectType() != bullet) {
				attached_objects[i]->setPosition(this->getPosition() + (attached_objects[i]->getPosition() - this->getPosition()).getRotated(difference));
			}
		}
		add_info.getCollisionModel()->changeAngle(difference);
	}

	ObjectType getObjectType() {
		return object_type_info;
	}

	UnitInfo * getUnitInfo() {
		return add_info.getUnitInfo();
	}

	void setAnimationType(AnimationType animation_type) {
		vis_info.animation_type = animation_type;
	}

	void deleteObject() {
		deleted = true;
		for (int i = 0; i < attached_objects.size(); i++) {
			attached_objects[i]->deleteObject();
		}
	}

	bool isDeleted() {
		return (deleted || (time_left == 0));
	}

	Point getSquareBorder() {
		return add_info.getCollisionModel()->getSquareBorder();
	}

	bool canObjectAttack() {
		if (getUnitInfo() == nullptr) {
			return false;
		}
		return getUnitInfo()->canObjectAttack();
	}

	void attachObject(Object * object) {
		attached_objects.push_back(object);
	}

	bool garbageCollector() {   // returns true if something worked
        if (float(time_left) > consts.getFPSLock() / fps.getFPS()) {
            time_left -= consts.getFPSLock() / fps.getFPS();
        }
        else if (time_left >= 0){
            time_left = 0;
        }

		bool cleaned = false;
		if (this->isDeleted() || this->getUnitInfo() == nullptr || this->getUnitInfo()->isDead()) {
			this->deleteObject();
			for (int i = 0; i < attached_objects.size(); i++) {
				attached_objects[i]->deleteObject();
				cleaned = true;
			}
			attached_objects.clear();
		}
		else {
			for (int i = 0; i < attached_objects.size(); i++) {
				if (attached_objects[i]->isDeleted() || attached_objects[i]->getUnitInfo() == nullptr || attached_objects[i]->getUnitInfo()->isDead()) {
					//attached_objects[i]->deleteObject();
					attached_objects.erase(attached_objects.begin() + i);
					cleaned = true;
				}
			}
		}
		return cleaned;
	}

	std::vector<Object *> * getAttached() {
		return &attached_objects;
	}

	void setFaction(int new_faction) {
		if (add_info.getUnitInfo()) {
			add_info.getUnitInfo()->setFaction(new_faction);
		}
		for (int i = 0; i < attached_objects.size(); i++) {
			if (attached_objects[i]->getObjectType() != bullet) {
				attached_objects[i]->setFaction(new_faction);
			}
		}
	}

	void setEffect(Effect effect) {
		if (add_info.getUnitInfo() != nullptr) {
			add_info.getUnitInfo()->setEffect(effect);
		}
	}

	void researchApply(int dome_count) {
		if (add_info.getUnitInfo()) {
			add_info.getUnitInfo()->researchApply(object_type_info, dome_count);
		}
	}

	void setParent(void * parent) {
		this->add_info.setParent(parent);
	}

	void * getParent() {
		return add_info.getParent();
	}

	void createNPCInfo(void * new_npc_info) {
		add_info.setNPCInfo(new_npc_info);
	}

	float getLifetime() { // 0 for infinite
		return std::max(0, time_left);
	}

	void setDiscovered() {
		discovered = true;
	}

	bool isDiscovered() {
		return discovered;
	}

	void initNPCInfo(void * new_npc_info) {
		if (!add_info.getNPCInfo()) {
			add_info.setNPCInfo(new_npc_info);
		}
	}

	void * getNPCInfo() {
		return add_info.getNPCInfo();
	}

    void setNPCInfo(void * npc_info) {
        if (!add_info.getNPCInfo()) {
            add_info.setNPCInfo(npc_info);
        }
    }

    Object * getDieObject() {
        return die_object;
    }

    void setDieObject(Object * new_die_object) {
        die_object = new_die_object;
    }

	float getAttachedPrice() {
		float output = 0;
		for (int i = 0; i < attached_objects.size(); i++) {
			switch (attached_objects[i]->getObjectType()) {
			case gold:
				output += consts.getBaseGoldPrice();
				break;
			case science:
				output += consts.getBaseSciencePrice();
				break;
			case dome:
				output += consts.getBaseDomePrice();
				break;
			case turret:
				output += consts.getBaseTurretPrice();
				break;
			}
		}
		return output;
	}

	void wipeAttached() {
		for (int i = 0; i < attached_objects.size(); i++) {
			attached_objects[i]->deleteObject();
		}
		attached_objects.clear();
	}

    bool attachmentFixed() {
        return attachment_fixed;
    }

    void setAttachmentFixState(bool state) {
        attachment_fixed = state;
    }

    void droneAttachmentFix() {
        if (this->getObjectSpriteType() == asteroid_drone_factory_sprite) {
            int cnt = 0;
            for (int i = 0; i < this->attached_objects.size(); i++) {
                Object * obj = attached_objects[i];

                if (obj->getObjectType() == drone) {
                    cnt++;
                }
            }
            int k = 0;
            for (int i = 0; i < this->attached_objects.size(); i++) {
                Object * obj = attached_objects[i];

                if (obj->getObjectType() == drone) {
                    float radius = obj->getCollisionModel()->getMaxRadius() + this->getCollisionModel()->getMaxRadius() + consts.getDroneFlightAddRange();
                    float angle = PI * (double)2 * (double)k / (double)cnt;
                    obj->setPosition(this->getPosition() + Point(cos(angle), sin(angle)) * radius);
                    k++;
                }
            }
        }
    }

    float getDroneCooldown() {
        return drone_factory_cooldown;
    }

    void setDroneCooldown() {
        drone_factory_cooldown = consts.getDroneCooldown();
    }

    void droneCooldownDecrement() {
        drone_factory_cooldown -= fps.getFPS() / consts.getFPSLock();
    }
};

bool checkObjectCollision(Object * obj1, Object * obj2) {
    if (!obj1->getCollisionModel() || !obj2->getCollisionModel()) {
        return false;
    }
	if ((obj1->getCollisionModel()->getPosition() - obj2->getCollisionModel()->getPosition()).getLength() > (obj1->getCollisionModel()->getMaxRadius() + obj2->getCollisionModel()->getMaxRadius())) {
		return false;
	}
	return checkModelCollision(obj1->getCollisionModel(), obj2->getCollisionModel());
}