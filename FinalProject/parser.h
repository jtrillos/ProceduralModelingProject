#ifndef __PARSER_H
#define __PARSER_H

#include <iostream>
#include <vector>
#include <random>
#include <math.h>  
#include "modeler.h"

using namespace std;


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

class parser {
private:
	// Variables
	std::string fileName; // File with rules

	// Methods
	void initModel();
	vector<std::string> readLines(std::string rulesFile); // Read lines from file
	vector<rule> parseProb(vector<std::string> str);
	double createRandom();
	rule searchClosest(vector<rule> vec, double key);
	vector<rule> eraseLosers(vector<rule> vec, vector<rule> temp);
	TypeObject stringToType(string str);
	void ruleToModel(rule r);
	vector<std::string> splitString(std::string str, char delimeter, char delimeter2);
	bool startsWith(std::string str, std::string prefix);
	vector<float> parseArguments(string token);
	vector<string> parseParameters(string token);

public:
	// Methods
	parser(std::string rulesFile); // Constructor
	vector<modeler> parseRules();

};

#endif