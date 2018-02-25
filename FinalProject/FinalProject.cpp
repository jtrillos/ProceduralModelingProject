// FinalProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "parser.h"
#include "modeler.h"

#include <fstream>

using namespace std;



int main()
{
	parser parser("rule.txt");
	vector<rule> rules = parser.parseRules();
	modeler model;
	model.ruleToModel(rules);
	modeler* data = model.GenerateModel();
	vector<modeler*> tree = model.printTree(data);
	return 0;
}

