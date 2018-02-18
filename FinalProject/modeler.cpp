#include "modeler.h"
#include "common.h"
#include <string>
#include "math.h"
#include<iostream>
#include <deque>
#include <vector>

using namespace std;

const float PI = 3.14159265358979323846;

modeler::modeler(string name, vector3d scopePosition, vector3d size, TypeObject type) {
	this->name = name;
	this->scopePosition = scopePosition;
	this->size = size;
	this->type = type;
}

modeler &modeler::translate(vector3d translation) {
	this->scopePosition.addElement(translation);
	return *this;
}

vector3d& modeler::getScopePosition() { 
	return this->scopePosition; 
}

vector3d& modeler::getSize() { 
	return this->size; 
}

TypeObject modeler::getType() { 
	return this->type; 
}

void modeler::setType(TypeObject type) { 
	this->type = type; 
}

string modeler::getName() {
	return this->name;
}

void modeler::setName(string name) {
	this->name = name;
}

vector3d modeler::rotate_around_axis(vector3d &size, float degrees, int axis) {
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

modeler &modeler::rotate(vector3d angles) {
	vector3d rotatedX = rotate_around_axis(this->size, angles.getX(), 0);
	vector3d rotatedY = rotate_around_axis(rotatedX, angles.getY(), 1);
	vector3d rotatedZ = rotate_around_axis(rotatedY, angles.getZ(), 2);
	this->size.set(rotatedZ);
	return *this;
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

vector<modeler> modeler::split(int axis, vector<float> ratios, vector<string> newmodelerNames) {
	vector<modeler> successors;
	if (ratios.size() != newmodelerNames.size()) {
		runtime_error("ERROR: Split. Ratios and Names should have the same size");
		successors.push_back(*this);
		return successors;
	}
	vector3d newPosition = this->scopePosition.copy();;
	vector3d newSize = this->size.copy();
	float positionChange = this->scopePosition.getElement(axis);

	for (int i = 0; i < ratios.size(); i++) {
		newSize.setElement(axis, ratios[i]);
		if (i != 0)
			positionChange += ratios[i - 1];
		newPosition.setElement(axis, positionChange);
		modeler newmodeler = modeler(newmodelerNames[i], newPosition, newSize, this->type);
		successors.push_back(newmodeler);
	}
	return successors;
}

vector<modeler> modeler::repeat(int axis, int times, string newmodelersName) {
	vector<modeler> successors;
	float ratio = this->size.getElement(axis) / times;

	vector3d newPosition = this->scopePosition.copy();
	vector3d newSize = this->size.copy();

	float initPosition = this->scopePosition.getElement(axis);
	for (int i = 0; i<times; i++) {
		newPosition.setElement(axis, initPosition + i * ratio);
		newSize.setElement(axis, ratio);
		modeler newmodeler(newmodelersName, newPosition, newSize, this->type);
		successors.push_back(newmodeler);
	}
	return successors;
}

vector<modeler> modeler::componentSplit(string type, vector<string> newmodelerNames) {
	vector<modeler> newmodeler;
	auto position = this->getScopePosition();
	auto size = this->getSize();
	// Right
	auto rp = position.copy();
	auto rs = size.copy();
	rp.setElement(2, rp.getZ() + rs.getZ());
	rs.setElement(2, -1);
	newmodeler.push_back(modeler(newmodelerNames[0], rp, rs, SCOPE));
	// Back
	auto bp = position.copy();
	auto bs = size.copy();
	bp.setElement(0, bp.getX() + bs.getX());
	bs.setElement(0, -1);
	newmodeler.push_back(modeler(newmodelerNames[1], bp, bs, SCOPE));
	// Left
	auto lp = position.copy();
	auto ls = size.copy();
	ls.setElement(2, -3);
	newmodeler.push_back(modeler(newmodelerNames[2], lp, ls, SCOPE));
	// Down
	auto bbp = position.copy();
	auto bbs = size.copy();
	bbs.setElement(1, -3);
	newmodeler.push_back(modeler(newmodelerNames[3], bbp, bbs, SCOPE));
	if (type.compare("4sides") != 0) {
		// Up
		auto tp = position.copy();
		auto ts = size.copy();
		tp.setElement(1, tp.getY() + ts.getY());
		ts.setElement(1, -1);
		newmodeler.push_back(modeler(newmodelerNames[4], tp, ts, SCOPE));
		if (type.compare("5sides") != 0) {
			// Front
			auto fp = position.copy();
			auto fs = size.copy();
			fs.setElement(0, -3);
			newmodeler.push_back(modeler(newmodelerNames[5], fp, fs, SCOPE));
		}
	}

	return newmodeler;
}

ostream &operator<<(ostream &strm, const modeler &modeler) {
	return strm << "modeler(" << modeler.name << "," << modeler.scopePosition << "," << modeler.size << "," << modeler.type << ")";
}

Tree::Tree() {
	this->root = NULL;
}

vector<Node *> Tree::applyRules(Node *current, vector<function<vector<modeler>(modeler)>> rules) {
	vector<modeler> successors;
	for (int i = 0; i < rules.size(); i++) {
		vector<modeler> result = rules[i](current->getModel());
		successors.reserve(successors.size() + result.size());
		successors.insert(successors.end(), result.begin(), result.end());
	}
	for (int i = 0; i < successors.size(); i++) {
		vector<Node *> children;
		Node *child = new Node(successors[i], children);
		current->addChild(child);
	}
	return current->getChildren();
}

void Tree::buildTree(vector<function<vector<modeler>(modeler)>> rules, modeler &axiom) {
	deque<Node *> modelQueue;
	vector<Node *> children;
	Node *current = new Node(axiom, children);
	this->root = current;
	modelQueue.push_back(current);

	while (!modelQueue.empty()) {
		current = modelQueue.front();

		vector<Node *> children = applyRules(current, rules);
		for (int j = 0; j < children.size(); j++) {
			if (children[j]->getModel().getType() == SCOPE)
				modelQueue.push_back(children[j]);

			else if (children[j]->getModel().getType() != INACTIVE)
				this->leafNodes.push_back(children[j]->getModel());
		}
		current->getModel().setType(INACTIVE);
		modelQueue.pop_front();
	}
	return;
}

vector<modeler> Tree::getLeafNodes() {
	return this->leafNodes;
}

Node* Tree::getRoot() {
	return  this->root;
};