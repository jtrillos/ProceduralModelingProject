#ifndef __COMMON_H
#define __COMMON_H
#include<iostream>
#include <vector>
#include <string>

using namespace std;

/*
* Here you can find all the common functionalities or data types neccessary
*/

enum TypeObject { INACTIVE, CUBE, CYLINDER, SCOPE, PLAIN, SOFA, TABLE, CABINET, CHAIR, TOY };


vector<std::string> splitString(std::string str, char delimeter, char delimeter2);
bool startsWith(std::string str, std::string prefix);
vector<string> parseParameters(string token);

class rule {
public:
	// Variables
	std::string head;
	std::string rules;
	double probability;

	// Methods
	rule();
	rule(std::string head, std::string rules);
	rule(std::string head, std::string rules, double probability);
	~rule();
};

class vector3d {
private:
	// Variables
	float x;
	float y;
	float z;

public:
	// Methods
	vector3d();
	vector3d(float x, float y, float z); // Constructor
	vector3d copyVector();
	float getElement(int axis); // Get elements of a specific axis (X=0, Y=1, Z=2)
	void setElement(int axis, float value);
	void addElement(vector3d& that);
	float getX();
	float getY();
	float getZ();
	void setVector(vector3d& that);
};


#endif