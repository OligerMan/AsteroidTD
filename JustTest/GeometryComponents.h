#pragma once
#include <SFML\Graphics.hpp>

#include <math.h>

#define PI 3.14159265

struct Point {
	double x = 0.0, y = 0.0;

	Point(const sf::Vector2i & vector) : x(vector.x), y(vector.y) {}
	Point(const sf::Vector2f & vector) : x(vector.x), y(vector.y) {}
	
	Point & operator = (const Point & p) = default;

	Point operator+(Point & b) {
		Point point = *this;
		point.x += b.x;
		point.y += b.y;

		return point;
	}

	Point operator-(Point & b) {
		Point point = *this;
		point.x -= b.x;
		point.y -= b.y;

		return point;
	}

	Point operator*(double b) {
		Point point = *this;
		point.x *= b;
		point.y *= b;

		return point;
	}

	Point operator/(double b) {
		Point point = *this;
		point.x /= b;
		point.y /= b;

		return point;
	}

	void operator+=(Point & b) {
		x += b.x;
		y += b.y;
	}

	void operator-=(Point & b) {
		x -= b.x;
		y -= b.y;
	}

	void operator*=(double b) {
		x *= b;
		y *= b;
	}

	void operator/=(double b) {
		x /= b;
		y /= b;
	}

	bool operator!=(Point & b) {
		if (b.x == x && b.y == y) {
			return false;
		}
		else {
			return true;
		}
	}

	bool operator==(Point & b) {
		if (b.x == x && b.y == y) {
			return true;
		}
		else {
			return false;
		}
	}

	Point(double x, double y) : x(x), y(y) {};
	Point() {}

	double getLength() {
		return sqrt(x * x + y * y);
	}

	Point normalize() {
		double length = getLength();
		if (length == 0) {
			return Point();
		}
		x /= length;
		y /= length;

		return Point(x,y);
	}

	Point getNormal() {
		Point normalized;
		normalized = *this;
		normalized.normalize();
		return normalized;
	}

	void rotate(double angle) {
		angle = angle * PI / 180;
		Point rotated_point = Point(x, y);
		rotated_point.x = x * cos(angle) - y * sin(angle);
		rotated_point.y = x * sin(angle) + y * cos(angle);
		x = rotated_point.x;
		y = rotated_point.y;
	}

};


struct Circle {
	Point center;
	double collision_radius = 0;

	Circle() {};

	Circle(double x, double y, double collision_radius) : collision_radius(collision_radius) {
		center.x = x;
		center.y = y;
	};

	Circle(Point position, double radius) : Circle(position.x, position.y, radius) {}

	~Circle() {}

	void rotateCircle(double angle) {
		center.rotate(angle);
	}

	bool checkCollision(const Circle circle, Point self_origin, double self_angle, Point second_origin, double second_angle) {
		Circle first_circle = *this, second_circle = circle;

		first_circle.rotateCircle(self_angle);
		second_circle.rotateCircle(second_angle);

		double x_shift = (self_origin.x + first_circle.center.x) - (second_origin.x + second_circle.center.x);
		double y_shift = (self_origin.y + first_circle.center.y) - (second_origin.y + second_circle.center.y);
		double radius_sum = first_circle.collision_radius + second_circle.collision_radius;

		return ((x_shift * x_shift) + (y_shift * y_shift) - (radius_sum * radius_sum)) < 0;
	}
};

bool circleCollision(Circle c1, Point orig1, double ang1, Circle c2, Point orig2, double ang2) {
	return c1.checkCollision(c2, orig1, ang1, orig2, ang2);
}