#include "common.h"

using namespace std;

// Split std::string in order to parse each part of the rule
vector<std::string> splitString(std::string line, char del, char del2) {
	vector<std::string> wordsLine;
	unsigned int lastPos = 0;
	for (unsigned int i = 0; i < line.length(); i++) {
		if (line[i] == del || line[i] == del2) {
			wordsLine.push_back(line.substr(lastPos, i - lastPos));
			lastPos = i + 1;
		}
	}
	if (lastPos != line.length())
		wordsLine.push_back(line.substr(lastPos, line.length() - lastPos));
	return wordsLine;
}

// Compare words in order to find the specific prefix (e.g. T, R, Subdiv...)
bool startsWith(std::string str, std::string prefix) {
	return str.compare(0, prefix.length(), prefix) == 0; // Return TRUE or FALSE
}

// Extract the arguments
vector<string> parseParameters(string token) {
	return splitString(splitString(token, '{', '}')[1], ',', ',');
}

// Class rule

// Constructor
rule::rule() {

}

rule::rule(std::string head, std::string rules) {
	this->head = head;
	this->rules = rules;
	this->probability = 1;
}

rule::rule(std::string head, std::string rules, double probability) {
	this->head = head;
	this->rules = rules;
	this->probability = probability;
}

rule::~rule() {

}

// Class vector3d

vector3d::vector3d() {
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

vector3d::vector3d(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
};

vector3d vector3d::copyVector() {
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

float vector3d::getX() { 
	return this->x; 
}

float vector3d::getY() { 
	return this->y; 
}

float vector3d::getZ() { 
	return this->z; 
}

void vector3d::setVector(vector3d &vector) {
	this->x = vector.getX();
	this->y = vector.getY();
	this->z = vector.getZ();
}