#ifndef __PARSER_H
#define __PARSER_H

#include <iostream>
#include <vector>
#include <random>
#include <math.h>  
#include "modeler.h"

using namespace std;


class parser {
private:
	// Variables
	std::string fileName; // File with rules

	// Methods
	vector<std::string> readLines(std::string rulesFile); // Read lines from file
	vector<rule> parseProb(vector<std::string> str);
	double createRandom();
	rule searchClosest(vector<rule> vec, double key);
	vector<rule> eraseLosers(vector<rule> vec, vector<rule> temp);

public:
	// Methods
	parser(std::string rulesFile); // Constructor
	vector<rule> parseRules();
};

#endif