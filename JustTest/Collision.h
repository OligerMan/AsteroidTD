#pragma once

#include <vector>

#include "GeometryComponents.h"
#include "CollisionParser.h"

/*
	There are will be only really simple collisions because 
	i dont want to spend my time and user's processor time on it
*/

class CollisionModel {
	Point position;
	Point origin;
	Point square_border;
	double angle = 0;

	Point speed;
	bool is_static = false;

	std::vector<Circle> col_model;

public:

	CollisionModel() {}

	CollisionModel(std::string object_path, bool * status) {
		col_model = collisionParse(object_path, is_static, status);
	}

	bool isStatic() {
		return is_static;
	}

	int getModelSize() {
		return col_model.size();
	}

	void addCircle(Point position, double radius) {
		col_model.push_back(Circle(position, radius));
	}

	Circle * getModelElem(int elem_num) {
		if (elem_num >= 0 && col_model.size() > elem_num) {
			return  &col_model[elem_num];
		}
		else {
			return nullptr;
		}
	}

	Point getOrigin() {
		return origin;
	}

	void setOrigin(Point new_origin) {
		origin = new_origin;
	}

	void changeOrigin(Point difference) {
		origin += difference;
	}

	void setAutoOrigin() {
		if (col_model.size() == 0) {
			return;
		}
		double left, right, top, bottom;
		if (!col_model.empty()) {
			left = col_model[0].center.x - col_model[0].collision_radius;
			right = col_model[0].center.x + col_model[0].collision_radius;
			top = col_model[0].center.y - col_model[0].collision_radius;
			bottom = col_model[0].center.y + col_model[0].collision_radius;
		}
		for (int i = 1; i < col_model.size(); i++) {
			if (col_model[i].center.x - col_model[i].collision_radius < left) {
				left = col_model[i].center.x - col_model[i].collision_radius;
			}
			if (col_model[i].center.x + col_model[i].collision_radius > right) {
				right = col_model[i].center.x + col_model[i].collision_radius;
			}
			if (col_model[i].center.y - col_model[i].collision_radius < top) {
				top = col_model[i].center.y - col_model[i].collision_radius;
			}
			if (col_model[i].center.y + col_model[i].collision_radius > bottom) {
				bottom = col_model[i].center.y + col_model[i].collision_radius;
			}
		}
		square_border = Point(right - left, bottom - top);
		origin = Point((right - left) / 2.0, (bottom - top) / 2.0);
	}

	Point getPosition() {
		return position;
	}

	void setPosition(Point new_position) {
		position = new_position;
	}

	void changePosition(Point difference) {
		if(!is_static){
			position += difference;
		}
	}

	void forceChangePosition(Point difference) {
		position += difference;
	}
	
	double getAngle() {
		return angle;
	}

	void setAngle(double new_angle) {
		angle = new_angle;
	}

	void changeAngle(double difference) {
		angle += difference;
	}

	Point getSpeed() {
		return speed;
	}

	void setSpeed(Point new_speed) {
		speed = new_speed;
	}

	void changeSpeed(Point difference) {
		speed += difference;
	}

	Point getSquareBorder(){
		return square_border;
	}
};

bool checkModelCollision(CollisionModel * m1, CollisionModel * m2) {
	unsigned int m1_size = m1->getModelSize();
	unsigned int m2_size = m2->getModelSize();

	Circle * c1, *c2;

	for (int i = 0; i < m1_size; i++) {
		for (int j = 0; j < m2_size; j++) {
			Point o1, o2;
			double a1, a2;

			c1 = m1->getModelElem(i);
			c2 = m2->getModelElem(j);
			o1 = m1->getPosition();
			o2 = m2->getPosition();
			a1 = m1->getAngle();
			a2 = m2->getAngle();

			if (circleCollision(*c1, o1, a1, *c2, o2, a2)) {
				return true;
			}
		}
	}
	return false;
}