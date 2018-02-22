#include "modeler.h"
#include "common.h"
#include <string>
#include "math.h"
#include<iostream>
#include <deque>
#include <vector>

using namespace std;

const float PI = 3.14159265358979323846;

modeler::modeler() {
	this->symbolPosition = vector3d(0, 0, 0);
	this->type = SCOPE;

}

modeler::modeler(string name, vector3d position, vector3d size, string texture, TypeObject type) {
	this->name = name;
	this->symbolPosition = position;
	this->size = size;
	this->texture = texture;
	this->type = type;
}

modeler &modeler::setSize(vector3d newSize) {
	if (newSize.getX() > -1000)
		this->size.setElement(0, newSize.getX());
	if (newSize.getY() > -1000)
		this->size.setElement(1, newSize.getY());
	if (newSize.getZ() > -1000)
		this->size.setElement(2, newSize.getZ());
	return *this;
}

vector3d& modeler::getSize() {
	return this->size;
}

void modeler::setType(TypeObject type) {
	this->type = type;
}

TypeObject modeler::getType() {
	return this->type;
}

vector3d& modeler::getSymbolPosition() {
	return this->symbolPosition;
}

void modeler::setName(string name) {
	this->name = name;
}

string modeler::getName() {
	return this->name;
}

void modeler::setTexture(string texture) {
	this->name = texture;
}

string modeler::getTexture() {
	return this->texture;
}

modeler& modeler::getModel() {
	return *this;
}

vector<modeler> modeler::componentSplit(string type, vector<string> newModelNames) {
	vector<modeler> newModel;
	auto position = this->getSymbolPosition();
	auto size = this->getSize();
	// Right
	auto rp = position.copyVector();
	auto rs = size.copyVector();
	rp.setElement(2, rp.getZ() + rs.getZ());
	rs.setElement(2, -1);
	newModel.push_back(modeler(newModelNames[0], rp, rs, newModelNames[0], SCOPE));
	// Back
	auto bp = position.copyVector();
	auto bs = size.copyVector();
	bp.setElement(0, bp.getX() + bs.getX());
	bs.setElement(0, -1);
	newModel.push_back(modeler(newModelNames[1], bp, bs, newModelNames[1], SCOPE));
	// Left
	auto lp = position.copyVector();
	auto ls = size.copyVector();
	ls.setElement(2, -3);
	newModel.push_back(modeler(newModelNames[2], lp, ls, newModelNames[2], SCOPE));
	// Down
	auto bbp = position.copyVector();
	auto bbs = size.copyVector();
	bbs.setElement(1, -3);
	newModel.push_back(modeler(newModelNames[3], bbp, bbs, newModelNames[3], SCOPE));
	if (type.compare("4sides") != 0) {
		// Up
		auto tp = position.copyVector();
		auto ts = size.copyVector();
		tp.setElement(1, tp.getY() + ts.getY());
		ts.setElement(1, -1);
		newModel.push_back(modeler(newModelNames[4], tp, ts, newModelNames[4], SCOPE));
		if (type.compare("5sides") != 0) {
			// Front
			auto fp = position.copyVector();
			auto fs = size.copyVector();
			fs.setElement(0, -3);
			newModel.push_back(modeler(newModelNames[5], fp, fs, newModelNames[5], SCOPE));
		}
	}

	return newModel;
}

vector<modeler> modeler::split(string head, int axis, vector<float> ratios, vector<string> newModelNames) {
	vector<modeler> successors;
	if (ratios.size() != newModelNames.size()) {
		runtime_error("ERROR: Split. Ratios and Names should have the same size");
		successors.push_back(*this);
		return successors;
	}
	vector3d newPosition = this->symbolPosition.copyVector();;
	vector3d newSize = this->size.copyVector();
	float positionChange = this->symbolPosition.getElement(axis);

	for (int i = 0; i < ratios.size(); i++) {
		newSize.setElement(axis, ratios[i]);
		if (i != 0)
			positionChange += ratios[i - 1];
		newPosition.setElement(axis, positionChange);
		modeler newModel = modeler(head, newPosition, newSize, newModelNames[i], this->type);
		successors.push_back(newModel);
	}
	return successors;
}

vector<modeler> modeler::repeat(string head, int axis, int times, string newModelsName) {
	vector<modeler> successors;
	float ratio = this->size.getElement(axis) / times;

	vector3d newPosition = this->symbolPosition.copyVector();
	vector3d newSize = this->size.copyVector();

	float initPosition = this->symbolPosition.getElement(axis);
	for (int i = 0; i<times; i++) {
		newPosition.setElement(axis, initPosition + i * ratio);
		newSize.setElement(axis, ratio);
		modeler newModel(head, newPosition, newSize, newModelsName, this->type);
		successors.push_back(newModel);
	}
	return successors;
}

modeler &modeler::translate(vector3d translation) {
	this->symbolPosition.addElement(translation);
	return *this;
}

modeler &modeler::rotate(vector3d angles) {
	vector3d rotatedX = rotate_axis(this->size, angles.getX(), 0);
	vector3d rotatedY = rotate_axis(rotatedX, angles.getY(), 1);
	vector3d rotatedZ = rotate_axis(rotatedY, angles.getZ(), 2);
	this->size.setVector(rotatedZ);
	return *this;
}

vector3d modeler::rotate_axis(vector3d &size, float degrees, int axis) {
	float angle = PI * degrees / 180.0;
	float cos_angle = cos(angle);
	float sin_angle = sin(angle);
	float posX = size.getX();
	float posY = size.getY();
	float posZ = size.getZ();
	float x, y, z;
	switch (axis) {
	case 0: 
		x = cos_angle * posX - sin_angle * posY;
		y = sin_angle * posX + cos_angle * posY;
		z = posZ;
		break;
	case 1: 
		x = cos_angle * posX + sin_angle * posZ;
		y = posY;
		z = -sin_angle * posX + cos_angle * posZ;
		break;
	case 2: 
		x = posX;
		y = cos_angle * posY - sin_angle * posZ;
		z = sin_angle * posY + cos_angle * posZ;
		break;
	default:
		break;
	}

	return vector3d(x, y, z);
}

