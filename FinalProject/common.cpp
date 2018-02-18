#include "common.h"

using namespace std;

vector3d::vector3d(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
};

vector3d vector3d::copy() {
	return vector3d(this->x, this->y, this->z);
}

float vector3d::getElement(int axis) {
	switch (axis) {
	case 0:
		return this->x;
	case 1:
		return this->y;
	case 2:
		return this->z;
	default:
		throw invalid_argument("No such axis!");
	}
}

void vector3d::setElement(int axis, float value) {
	switch (axis) {
	case 0:
		this->x = value;
		break;
	case 1:
		this->y = value;
		break;
	case 2:
		this->z = value;
		break;
	default:
		throw invalid_argument("No such axis!");
	}
}

void vector3d::addElement(vector3d& that) {
	this->x += that.x;
	this->y += that.y;
	this->z += that.z;
}

float vector3d::getX() { return this->x; }

float vector3d::getY() { return this->y; }

float vector3d::getZ() { return this->z; }

void vector3d::set(vector3d &vector) {
	this->x = vector.getX();
	this->y = vector.getY();
	this->z = vector.getZ();
}

ostream &operator<<(ostream &strm, const vector3d &vector) {
	return strm << "V(" << vector.x << "," << vector.y << "," << vector.z << ")";
}
