// FinalProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "parser.h"

using namespace std;


int main()
{
	parser parser("rule.txt");
	vector<modeler> model = parser.parseRules();
	return 0;
}

