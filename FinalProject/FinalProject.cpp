// FinalProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "parser.h"
#include "modeler.h"

using namespace std;

void print_vector(vector<modeler> vec) {
	for (int i = 0; i<vec.size(); i++) {
		cout << vec[i] << endl;
	}
	cout << endl;
}

int main()
{
	parser parser("rule.txt");
	auto rules = parser.parseRules();
	Tree tree;
	modeler m = modeler("main", vector3d(0, 0, 0), vector3d(40, 120, 80), SCOPE);
	tree.buildTree(rules, m);
	auto nodes = tree.getLeafNodes();
	print_vector(nodes);
	return 0;
}

