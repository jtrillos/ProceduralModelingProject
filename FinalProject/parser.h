#ifndef __PARSER_H
#define __PARSER_H

#include <iostream>
#include <vector>
#include <functional>
#include <random>
#include <math.h>  
#include "modeler.h"

using namespace std;

class parser {
public:
	string fileName; // File with rules

	parser(string rulesFile); // Constructor

	vector<string> readLines(string rulesFile); // Read lines from file

	vector<string> splitString(string str, char delimeter, char delimeter2);
	bool startsWith(string str, string prefix);

	TypeObject stringToType(string str);
	function<vector<modeler>(modeler)> stringToRule(string string);

	vector<function<float()>> parseArguments(string token);
	vector<string> parseParameters(string token);
	vector<function<vector<modeler>(modeler)>> parseRules();
};

#endif