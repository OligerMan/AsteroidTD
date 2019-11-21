#pragma once

#include "Collision.h"
#include "ObjectTypes.h"
#include "VisualInfo.h"
#include "UnitInfo.h"

class Object {

	CollisionModel * col_model = new CollisionModel();

	CollisionType object_col_type;
	VisualInfo vis_info;
	UnitInfo * unit_info;
	ObjectType object_type_info;

	bool deleted = false;
	std::vector<Object *> attached_objects;
	int time_left = -1;

public:

	~Object() {
		delete col_model;
		delete unit_info;
		for (int i = 0; i < attached_objects.size(); i++) {
			attached_objects[i]->deleteObject();
		}
		attached_objects.clear();
		unit_info = nullptr;
		col_model = nullptr;
	}

	Object() {}

	Object(
		Point point
	) {
		col_model->setPosition(point);
		col_model->addCircle(Point(), 0);
		time_left = lifetime[object_type_info];
	}

	Object(
		Point point, 
		Point origin
	) : Object(
		point
	) {
		col_model->setOrigin(origin);
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

		delete col_model;

		col_model = new CollisionModel(collision_info_set[new_object_type]);
		col_model->setPosition(point);
		col_model->setOrigin(origin);
		
		unit_info = new UnitInfo(unit_info_set[new_object_type]);

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
		unit_info->setHealth(hp);
		unit_info->setMana(mana);
		unit_info->setEndurance(endurance);
		time_left = lifetime[object_type_info];
	}

	CollisionModel * getCollisionModel() {
		return col_model;
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
		if (isDeleted() || !col_model) {
			return Point();
		}
		return col_model->getSpeed();
	}

	void setSpeed(Point speed) {
		if (isDeleted() || !col_model) {
			return;
		}
		col_model->setSpeed(speed);
	}

	void changeSpeed(Point difference) {
		if (isDeleted() || !col_model) {
			return;
		}
		col_model->changeSpeed(difference);
	}

	Point getPosition() {
		if (isDeleted() || !col_model) {
			return Point();
		}
		return col_model->getPosition();
	}

	void setPosition(Point origin) {
		if (isDeleted() || !col_model) {
			return;
		}
		for (int i = 0; i < attached_objects.size(); i++) {
			attached_objects[i]->setPosition(origin + (getPosition() - attached_objects[i]->getPosition()));
		}
		col_model->setPosition(origin);
	}

	void changePosition(Point difference) {
		if (isDeleted() || !col_model) {
			return;
		}
		for (int i = 0; i < attached_objects.size(); i++) {
			attached_objects[i]->changePosition(difference);
		}
		col_model->changePosition(difference);
	}

	void forceChangePosition(Point difference) {
		if (isDeleted() || !col_model) {
			return;
		}
		for (int i = 0; i < attached_objects.size(); i++) {
			attached_objects[i]->forceChangePosition(difference);
		}
		col_model->forceChangePosition(difference);
	}

	Point getOrigin() {
		if (isDeleted() || !col_model) {
			return Point();
		}
		return col_model->getOrigin();
	}

	void setOrigin(Point origin) {
		if (isDeleted() || !col_model) {
			return;
		}
		col_model->setOrigin(origin);
	}

	void changeOrigin(Point difference) {
		if (isDeleted() || !col_model) {
			return;
		}
		col_model->changeOrigin(difference);
	}

	void setAutoOrigin() {
		if (isDeleted() || !col_model) {
			return;
		}
		col_model->setAutoOrigin();
	}

	double getAngle() {
		if (isDeleted() || !col_model) {
			return 0;
		}
		return col_model->getAngle();
	}

	void setAngle(double angle) {
		if (isDeleted() || !col_model) {
			return;
		}
		col_model->setAngle(angle);
	}

	void changeAngle(double difference) {
		if (isDeleted() || !col_model) {
			return;
		}
		col_model->changeAngle(difference);
	}

	ObjectType getObjectType() {
		return object_type_info;
	}

	UnitInfo * getUnitInfo() {
		return unit_info;
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
		return col_model->getSquareBorder();
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
		time_left--;
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
		unit_info->setFaction(new_faction);
		for (int i = 0; i < attached_objects.size(); i++) {
			attached_objects[i]->setFaction(new_faction);
		}
	}

	void setEffect(Effect effect) {
		if (unit_info != nullptr) {
			unit_info->setEffect(effect);
		}
	}

	void researchApply(int dome_count) {
		if (unit_info) {
			unit_info->researchApply(object_type_info, dome_count);
		}
	}
};

bool checkObjectCollision(Object * obj1, Object * obj2) {
	if ((obj1->getCollisionModel()->getPosition() - obj2->getCollisionModel()->getPosition()).getLength() > (obj1->getCollisionModel()->getMaxRadius() + obj2->getCollisionModel()->getMaxRadius())) {
		return false;
	}
	/*ObjectType type1 = obj1->getObjectType(), type2 = obj1->getObjectType();
	if ((type2 != bullet && (type1 == dome || type1 == turret || type1 == science || type1 == gold)) || (type1 != bullet && (type2 == dome || type2 == turret || type2 == science || type2 == gold))){
		return false;
	}*/
	return checkModelCollision(obj1->getCollisionModel(), obj2->getCollisionModel());
}