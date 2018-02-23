#ifndef __MODELER_H
#define __MODELER_H

#include <iostream>
#include <vector>
#include "common.h"

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
	vector<modeler> children;

	// Methods
	modeler(); // Constructor
	modeler(string name);
	modeler(string name, vector3d symbolPosition, vector3d size, string texture, TypeObject type, vector<modeler> children);
	modeler(string name, vector3d symbolPosition, vector3d size, string texture, TypeObject type);

	modeler& setSize(vector3d newSize); // Add Size
	vector3d& getSize();
	void setType(TypeObject type); // Add type of object
	TypeObject getType();
	vector3d& getSymbolPosition(); // Get Position
	void setName(string name); // Add Name (head)
	string getName();
	void setTexture(string texture); // Add Name (head)
	string getTexture(); 
	modeler& getModel(); // Get Model
	void setChildren(vector<modeler> children); // Add children
	vector<modeler> getChildren();

	vector<modeler> componentSplit(string type, vector<string> newmodelerNames); // Splits the modeler into modelers of lesser dimensions
	vector<modeler> split(int axis, vector<float> ratios, vector<string> newmodelerNames); // Splits the current scope along one specific axis
	vector<modeler> repeat(string head, int axis, int times, string newmodelersName); // Allow for larger scale changes in the split rules, we often want to tile a specified element.
	modeler &translate(vector3d translation); // Translation vector
	modeler &rotate(vector3d angles); // Rotation 
	vector3d rotate_axis(vector3d &position, float degrees, int axe);
	
};

class Node {
private:
	// Variables
	modeler model;
	vector<Node *> children;

public:
	// Methods
	Node(modeler& model, vector<Node *> children) {
		this->model = model;
		this->children = children;
	}
	Node(const Node &other) {
		this->model = other.model;
		this->children = other.children;
	}
	modeler& getModel() {
		return this->model;
	}
	vector<Node*> getChildren() {
		return this->children;
	}
	void addChild(Node * child) {
		this->children.push_back(child);
	}
};

class Tree {
private:
	// Variables
	Node * root;
	vector<modeler> leafNodes;

public:
	// Methods
	Tree(); // Constructor
	vector<Node*> applyRules(Node* current, vector<modeler> rules);
	void buildTree(vector<modeler> rules, modeler& axiom);
	vector<modeler> getLeafNodes();
	Node* getRoot();
};

#endif