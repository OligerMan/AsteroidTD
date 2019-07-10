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

public:

	~Object() {
		delete col_model;
		delete unit_info;
		for (int i = 0; i < attached_objects.size(); i++) {
			attached_objects[i]->deleteObject();
		}
	}

	Object() {}

	Object(
		Point point
	) {
		col_model->setPosition(point);
		col_model->addCircle(Point(), 0);
	}

	Object(
		Point point, 
		Point origin
	) : Object(
		point
	) {
		col_model->setOrigin(origin);
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

		bool status;

		col_model = new CollisionModel("collision/" + collision_type[new_object_type] + ".col", &status);
		col_model->setPosition(point);
		col_model->setOrigin(origin);
		
		unit_info = new UnitInfo("unit_info/" + object_type[new_object_type] + ".unit", &status);
		if (!status) {
			delete unit_info;
			unit_info = nullptr;
		}
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
		return col_model->getSpeed();
	}

	void setSpeed(Point speed) {
		col_model->setSpeed(speed);
	}

	void changeSpeed(Point difference) {
		col_model->changeSpeed(difference);
	}

	Point getPosition() {
		return col_model->getPosition();
	}

	void setPosition(Point origin) {
		col_model->setPosition(origin);
	}

	void changePosition(Point difference) {
		col_model->changePosition(difference);
	}

	void forceChangePosition(Point difference) {
		col_model->forceChangePosition(difference);
	}

	Point getOrigin() {
		return col_model->getOrigin();
	}

	void setOrigin(Point origin) {
		col_model->setOrigin(origin);
	}

	void changeOrigin(Point difference) {
		col_model->changeOrigin(difference);
	}

	void setAutoOrigin() {
		col_model->setAutoOrigin();
	}

	double getAngle() {
		return col_model->getAngle();
	}

	void setAngle(double angle) {
		col_model->setAngle(angle);
	}

	void changeAngle(double difference) {
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
	}

	bool isDeleted() {
		return deleted;
	}

	Point getSquareBorder() {
		return col_model->getSquareBorder();
	}

	bool canObjectAttack() {
		if (getUnitInfo() == nullptr) {
			return false;
		}
		UnitInfo * unit = getUnitInfo();
		if (unit->getAttackRange1() == 0 && unit->getAttackRange2() == 0 && unit->getAttackRange3() == 0) {
			return false;
		}
		return true;
	}

	void attachObject(Object * object) {
		attached_objects.push_back(object);
	}

	void garbageCollector() {
		for (int i = 0; i < attached_objects.size(); i++) {
			if (attached_objects[i]->isDeleted() || !(attached_objects[i]->getUnitInfo() != nullptr && !attached_objects[i]->getUnitInfo()->isDead())) {
				attached_objects[i]->deleteObject();
				attached_objects.erase(attached_objects.begin() + i);
			}
		}
	}
};

bool checkObjectCollision(Object * obj1, Object * obj2) {
	return checkModelCollision(obj1->getCollisionModel(), obj2->getCollisionModel());
}