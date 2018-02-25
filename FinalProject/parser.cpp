#include <iterator>
#include <algorithm> 
#include <string>
#include <fstream>
#include <ctime>
#include "parser.h"

using namespace std;

// Import the file (that contains the rules) in the attribute fileName
// Constructor
parser::parser(std::string file) {
	this->fileName = file;
}

// Reads the file and extract the lines saving them in a vector
vector<std::string> parser::readLines(std::string file) {
	vector<std::string> linesFile;
	ifstream dict_file(file);
	std::string line;
	while (getline(dict_file, line))
		if (line[0] != '#') // Ignoring '#' that are use for comments
			linesFile.push_back(line);
	return linesFile;
}

// Extract the information from the file and converted in a set of rules
vector<rule> parser::parseRules() {
	vector<std::string> lines = readLines(this->fileName);
	vector<rule> rules = parseProb(lines);
	return rules;
}

// Select only the rules that win
vector<rule> parser::parseProb(vector<std::string> lines) {
	vector<rule> parseRules;
	vector<rule> temp; // Show the repeat heads
	vector<rule> winner; // Show only the winners
	for (auto line : lines) {
		auto keys = splitString(line, '=', ':');
		rule r = rule();
		if (keys.size() > 2) {
			r = rule(keys[0], keys[1], stod(keys[2]));
		}
		else {
			r = rule(keys[0], keys[1]);
		}
		parseRules.push_back(r);
	}

	bool flag = true;
	bool f = false;
	for (int n1 = 1; n1 < parseRules.size(); ++n1)
	{
		if (parseRules[n1 - 1].head != parseRules[n1].head) {
			if (f) {
				temp.push_back(parseRules[n1 - 1]);
				double  probability = createRandom();
				winner.push_back(searchClosest(temp, probability));
				temp.clear();
				// cout << probability << endl;
			}
			flag = false;
			f = false;
		}
		else {
			temp.push_back(parseRules[n1 - 1]);
			f = true;
		}
		if (n1 + 1 == parseRules.size() && f) {
			temp.push_back(parseRules[n1]);
			double  probability = createRandom();
			winner.push_back(searchClosest(temp, probability));
			temp.clear();
			// cout << probability << endl;
		}
	}
	parseRules = eraseLosers(parseRules, winner);
	return parseRules;
}

// Generate a random number between 0 and 1	
double parser::createRandom() {	
	srand(time(NULL));
	return ((double)(rand() % 100) + 1) / 100.0;
}

// Return the winner rules
rule parser::searchClosest(vector<rule> vec, double key){
	double value = abs(key - vec[0].probability);
	rule num = vec[0];
	for (int i = 0; i < vec.size(); i++){
		if (value > abs(key - vec[i].probability)){
			value = abs(key - vec[i].probability);
			num = vec[i];
		}
	}
	return num;
}

// Delete the losers rules
vector<rule> parser::eraseLosers(vector<rule> vec, vector<rule> temp) {
	for (int i = 0; i < temp.size(); i++){
		for (int j = 0; j < vec.size(); j++) {
			if ((vec[j].head == temp[i].head) && (vec[j].probability != temp[i].probability)) {
				vec.erase(vec.begin() + j);
			}
		}
	}
	return vec;
}