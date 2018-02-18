#ifndef __MODELER_H
#define __MODELER_H

#include <iostream>
#include <vector>
#include "common.h"
#include <functional>
#include <stack>

using namespace std;

class modeler {
	string name;
	vector3d scopePosition;
	vector3d size;
	TypeObject type; 
	friend ostream &operator<<(ostream &strm, const modeler &modeler);
	vector3d rotate_around_axis(vector3d &position, float degrees, int axe);

public:
	modeler() = default;
	modeler(string name, vector3d scopePosition, vector3d size, TypeObject type); // Constructor

	modeler &translate(vector3d translation); // Translation vector
	modeler &rotate(vector3d angles); // Rotation 

	modeler &setSize(vector3d newSize); // Add Size
	vector3d& getSize();
	void setType(TypeObject type); // Add type of object
	TypeObject getType();
	vector3d& getScopePosition(); // Get Position
	string getName(); // Get Name (predecesor)
	void setName(string name);

	vector<modeler> split(int axis, vector<float> ratios, vector<string> newmodelerNames); // Splits the current scope along one specific axis

	vector<modeler> repeat(int axis, int times, string newmodelersName); // Allow for larger scale changes in the split rules, we often want to tile a specified element.

	vector<modeler> componentSplit(string type, vector<string> newmodelerNames); // Splits the modeler into modelers of lesser dimensions
};

ostream &operator<<(ostream &strm, const modeler &modeler);


class Node {
	modeler model;
	vector<Node *> children;
public:
	Node(modeler& model, vector<Node *> children) {
		this->model = model;
		this->children = children;
	}
	Node(const Node &other) {
		this->model = other.model;
		this->children = other.children;
	}
	modeler & getModel() {
		return this->model;
	}
	vector<Node *> getChildren() {
		return this->children;
	}
	void addChild(Node * child) {
		this->children.push_back(child);
	}
};

class Tree {
	Node *root;
	vector<modeler> leafNodes;
public:
	Tree();
	vector<Node *> applyRules(Node * current, vector<function<vector<modeler>(modeler)>> rules);
	void buildTree(vector<function<vector<modeler>(modeler)>> rules, modeler& axiom);
	vector<modeler> getLeafNodes();
	Node * getRoot();
};

#endif