#include "modeler.h"
#include "common.h"
#include <string>
#include "math.h"
#include<iostream>
#include <deque>
#include <queue>
#include <vector>
#include <fstream>

using namespace std;

const float PI = 3.14159265358979323846;
vector<modeler*> list;
modeler* tree;

modeler::modeler() {
}

modeler::modeler(string name) {
	this->name = name;
	this->symbolPosition = vector3d(0, 0, 0);
	this->size = vector3d(0, 0, 0);
	this->texture = "";
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

modeler &modeler::setSymbolPosition(vector3d newPosition) {
	if (newPosition.getX() > -1000)
		this->symbolPosition.setElement(0, newPosition.getX());
	if (newPosition.getY() > -1000)
		this->symbolPosition.setElement(1, newPosition.getY());
	if (newPosition.getZ() > -1000)
		this->symbolPosition.setElement(2, newPosition.getZ());
	return *this;
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
	this->texture = texture;
}

string modeler::getTexture() {
	return this->texture;
}

modeler& modeler::getModel() {
	return *this;
}

void modeler::initModel() {
	tree = new modeler("main");
	list.push_back(tree);
}

// Identify the idObject returning the enum type
TypeObject modeler::stringToType(string str) {
	if (!str.compare("cube"))
		return CUBE;
	else if (!str.compare("cylinder"))
		return CYLINDER;
	else if (!str.compare("scope"))
		return SCOPE;
	else if (!str.compare("plain"))
		return PLAIN;
	else if (!str.compare("sofa"))
		return SOFA;
	else if (!str.compare("table"))
		return TABLE;
	else if (!str.compare("cabinet"))
		return CABINET;
	else if (!str.compare("chair"))
		return CHAIR;
	else if (!str.compare("toy"))
		return TOY;
	else runtime_error("ERROR: stringToType cannot parse");
}

// Read a string line and transformed it into a rule
void modeler::ruleToModel(vector<rule> r) {
	map<string,string> mapRule = ruleToMap(r); // I transform the vector<rule> into a map<string,string> in order to compare it with the temporal list
	initModel();
	while (list.size()) {
		modeler *currentmodel = list[0];
		list.erase(list.begin());
		string modelerName = currentmodel->getName();
		if (mapRule.find(modelerName) != mapRule.end()) {
			string rules = mapRule.at(modelerName);
			vector<string> keys = splitString(rules, ' ', ' ');
			for (int i = 0; i < keys.size(); i++) {
				if (startsWith(keys[i], "T")) { // Translation
					vector<float> args = parseArguments(keys[i], currentmodel);
					currentmodel->translate(vector3d(args[0], args[1], args[2]));
				}
				else if (startsWith(keys[i], "Subdiv")) { // Divide scope in smaller scopes (children)
					vector<float> args = parseArguments(keys[i], currentmodel);
					vector<string> parameters = parseParameters(keys[i]);
					int axis = round(args[0]);
					vector<float> ratios;
					for (int i = 1; i < args.size(); ratios.push_back(args[i++])); // Parsing ratios
					currentmodel->subDiv(axis, ratios, parameters, currentmodel);
				}
				else if (startsWith(keys[i], "S3d")) { // Set new size for an specific axis
					vector<float> args = parseArguments(keys[i], currentmodel);
					int axis = round(args[0]);
					vector3d size = currentmodel->getSize();
					size.setElement(axis, args[1]);
					currentmodel->setSize(size);
				}
				else if (startsWith(keys[i], "S")) { // Set new size
					vector<float> args = parseArguments(keys[i], currentmodel);
					currentmodel->setSize(vector3d(args[0], args[1], args[2]));
				}
				else if (startsWith(keys[i], "Comp")) { // Split scope into plains
					string type = splitString(keys[i], '(', ')')[1];
					vector<string> parameters = parseParameters(keys[i]);
					currentmodel->comp(type, parameters, currentmodel);
				}
				else if (startsWith(keys[i], "I")) { // Instance of a figure
					TypeObject type = stringToType(splitString(keys[i], '(', ')')[1]);
					string texture = parseParameters(keys[i])[0]; // Texture
					currentmodel->setTexture(texture);
					currentmodel->setType(type);
				}
				else if (startsWith(keys[i], "Repeat")) { // Splitting scope into the same objects
					vector<float> args = parseArguments(keys[i], currentmodel);
					string newmodelerName = parseParameters(keys[i])[0];
					int axis = round(args[0]);
					currentmodel->repeat(axis, round(args[1]), newmodelerName, currentmodel);
				}
				else if (startsWith(keys[i], "R")) { // Rotating around axis
					vector<float> args = parseArguments(keys[i], currentmodel);
					currentmodel->rotate(vector3d(args[0], args[1], args[2]));
				}
				else runtime_error("ERROR in parser : that command does not exist");
			}
		}
	}
}

//transform the vector<rule> in a map<string head,string rules>
map<string,string> modeler::ruleToMap(vector<rule> rules) {
	map<string,string> r;
	for (int i = 0; i < rules.size(); i++) {
		r.insert(std::pair<string, string>(rules[i].head, rules[i].rules));
	}
	return r;
}

// Extract the arguments (relative and absolute values)
vector<float> modeler::parseArguments(string token, modeler* parent) {
	vector<string> args = splitString(splitString(token, '(', ')')[1], ',', ',');
	vector<float> vec;
	for (int k = 0; k < args.size(); k++) {
		if (startsWith(args[k], "rnd")) {
			vector<string> rArgs = splitString(splitString(args[k], '<', '>')[1], '-', '-');
			vec.push_back(stof(rArgs[0]));
			vec.push_back(stof(rArgs[1]));
		}
		else if (startsWith(args[k], "r")) {
			int axis = vec[0];
			string number = args[k].substr(1, args[k].length());
			float size = parent->getSize().getElement(axis);;
			vec.push_back(stof(number)*size);
		}
		else {
			double value = stof(args[k]);
			vec.push_back(value);
		}
	}
	return vec;
}

void modeler::comp(string type, vector<string> newModelNames, modeler* parent) {
	auto position = parent->getSymbolPosition();
	auto size = parent->getSize();
	// Right
	auto rp = position.copyVector();
	auto rs = size.copyVector();
	rp.setElement(0, rp.getX() + rs.getX());
	rs.setElement(0, 0);
	modeler* modelRight = new modeler(newModelNames[0]);
	modelRight->setSymbolPosition(rp);
	modelRight->setSize(rs);
	parent->children.push_back(modelRight);
	list.push_back(modelRight);
	// Back
	auto bp = position.copyVector();
	auto bs = size.copyVector();
	bp.setElement(2, bp.getZ() + bs.getZ());
	bs.setElement(2, 0);
	modeler* modelBack = new modeler(newModelNames[1]);
	modelBack->setSymbolPosition(bp);
	modelBack->setSize(bs);
	parent->children.push_back(modelBack);
	list.push_back(modelBack);
	// Left
	auto lp = position.copyVector();
	auto ls = size.copyVector();
	ls.setElement(0, 0);
	modeler* modelLeft = new modeler(newModelNames[2]);
	modelLeft->setSymbolPosition(lp);
	modelLeft->setSize(ls);
	parent->children.push_back(modelLeft);
	list.push_back(modelLeft);
	// Down
	auto dp = position.copyVector();
	auto ds = size.copyVector();
	ds.setElement(1, 0);
	modeler* modelDown = new modeler(newModelNames[3]);
	modelDown->setSymbolPosition(dp);
	modelDown->setSize(ds);
	parent->children.push_back(modelDown);
	list.push_back(modelDown);
	if (type.compare("5sides") == 0) {
		// Up
		auto up = position.copyVector();
		auto us = size.copyVector();
		up.setElement(1, up.getY() + us.getY());
		us.setElement(1, 0);
		modeler* modelUp = new modeler(newModelNames[4]);
		modelUp->setSymbolPosition(up);
		modelUp->setSize(us);
		parent->children.push_back(modelUp);
		list.push_back(modelUp);
		if (type.compare("6sides") == 0) {
			// Front
			auto fp = position.copyVector();
			auto fs = size.copyVector();
			fs.setElement(2, 0);
			modeler* modelFront = new modeler(newModelNames[5]);
			modelFront->setSymbolPosition(fp);
			modelFront->setSize(fs);
			parent->children.push_back(modelFront);
			list.push_back(modelFront);
		}
	}
}

void modeler::subDiv(int axis, vector<float> ratios, vector<string> newModelNames, modeler* parent) {
	if (ratios.size() != newModelNames.size()) {
		runtime_error("ERROR: Split. Ratios and Names should have the same size");
	}
	else {
		vector3d newPosition = parent->symbolPosition.copyVector();;
		vector3d newSize = parent->size.copyVector();
		float positionChange = parent->symbolPosition.getElement(axis);

		for (int i = 0; i < ratios.size(); i++) {
			newSize.setElement(axis, ratios[i]);
			if (i != 0)
				positionChange += ratios[i - 1];
			newPosition.setElement(axis, positionChange);
			modeler* newModel = new modeler(newModelNames[i]);
			newModel->setSymbolPosition(newPosition);
			newModel->setSize(newSize);
			newModel->setType(parent->type);
			parent->children.push_back(newModel);
			list.push_back(newModel);
		}
	}
}

void modeler::repeat(int axis, int times, string newModelsName, modeler* parent) {
	float ratio = parent->size.getElement(axis) / times;

	vector3d newPosition = parent->symbolPosition.copyVector();
	vector3d newSize = parent->size.copyVector();

	float initPosition = parent->symbolPosition.getElement(axis);
	for (int i = 0; i<times; i++) {
		newPosition.setElement(axis, initPosition + i * ratio);
		newSize.setElement(axis, ratio);
		modeler* newModel = new modeler(newModelsName);
		newModel->setSymbolPosition(newPosition);
		newModel->setSize(newSize);
		newModel->setType(parent->type);
		parent->children.push_back(newModel);
		list.push_back(newModel);
	}
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

modeler* modeler::GenerateModel() {
	return tree;
}

vector<modeler*> modeler::printTree(modeler* data) {
	list.push_back(data); // list with the tree
	vector<modeler*> tree; 
	ofstream cout("result.txt");
	while (list.size()) {
		modeler* cur_item = list[0];
		list.erase(list.begin());
		tree.push_back(cur_item);

		cout << "Name: " << cur_item->getName() << endl;
		vector3d& position = cur_item->getSymbolPosition();
		cout << "Position: (" << position.getX() << ", " << position.getY() << ", " << position.getZ() << ")" << endl;
		vector3d& size = cur_item->getSize();
		cout << "Size: (" << size.getX() << ", " << size.getY() << ", " << size.getZ() << ")" << endl;
		cout << "Texture: " << cur_item->getTexture() << endl;
		cout << "Children: [ ";
		for (int i = 0; i<cur_item->children.size(); i++) {
			modeler *child = cur_item->children[i];
			cout << child->getName() << " ";
			list.push_back(child);
		}
		cout << "]" << endl << endl;
	}
	cout.close();
	return tree;

}
