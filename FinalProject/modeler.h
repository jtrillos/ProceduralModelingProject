#ifndef __MODELER_H
#define __MODELER_H

#include <iostream>
#include <vector>
#include <map>
#include "common.h"
#include "parser.h"

using namespace std;

class modeler {
private:
	// Variables
	string name;
	vector3d symbolPosition;
	vector3d size;
	string texture;
	TypeObject type;

public:	
	// Variables
	vector<modeler*> children;

	// Methods
	modeler(); // Constructor
	modeler(string name);
	modeler(string name, vector3d symbolPosition, vector3d size, string texture, TypeObject type);

	// Setters and getters
	modeler& setSize(vector3d newSize); // Add Size
	vector3d& getSize();
	void setType(TypeObject type); // Add type of object
	TypeObject getType();
	modeler& setSymbolPosition(vector3d newPosition); // Add Position
	vector3d& getSymbolPosition();
	void setName(string name); // Add Name (head)
	string getName();
	void setTexture(string texture); // Add Name (head)
	string getTexture(); 
	modeler& getModel(); // Get Model

	void initModel();
	TypeObject stringToType(string str);
	void ruleToModel(vector<rule> rules);
	map<string,string> ruleToMap(vector<rule> rules);
	vector<float> parseArguments(string token, modeler* parent);

	// Rules operators
	void comp(string type, vector<string> newmodelerNames, modeler* parent); // Splits the modeler into modelers of lesser dimensions
	void subDiv(int axis, vector<float> ratios, vector<string> newmodelerNames, modeler* parent); // Splits the current scope along one specific axis
	void repeat(int axis, int times, string newmodelersName, modeler* parent); // Allow for larger scale changes in the split rules, we often want to tile a specified element.
	modeler &translate(vector3d translation); // Translation vector
	modeler &rotate(vector3d angles); // Rotation 
	vector3d rotate_axis(vector3d &position, float degrees, int axe);

	modeler* GenerateModel();
	vector<modeler*> printTree(modeler* data);
	
};


#endif