#include "stdafx.h"
#include <iostream>
#include "parser.h"
#include "modeler.h"

using namespace std;

int main()
{
	// PARSING
	parser parser("rule.txt"); // Import the rules from the file
	vector<rule> rules = parser.parseRules(); // Parse the rules 
	// MODELING
	modeler model; 
	modeler* dataTree = model.ruleToModel(rules); // Create the data tree
	vector<modeler*> tree = model.printTree(dataTree); // Print the tree in a vector<modeler*>
	// RENDERING

	return 0;
}

