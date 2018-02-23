#include <iterator>
#include <algorithm> 
#include <string>
#include <fstream>
#include <ctime>
#include <queue>
#include "parser.h"

using namespace std;
vector<modeler> list;

// Constructor
rule::rule() {

}

rule::rule(std::string head, std::string rules) {
	this->head = head;
	this->rules = rules;
	this->probability = 1;
}

rule::rule(std::string head, std::string rules, double probability) {
	this->head = head;
	this->rules = rules;
	this->probability = probability;
}

rule::~rule() {

}

void parser::initModel() {
	modeler model = modeler("main");
	list.push_back(model);
}

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
vector<modeler> parser::parseRules() {
	vector<std::string> lines = readLines(this->fileName);
	vector<rule> rules = parseProb(lines);
	vector<modeler> model;
	for (int i = 0; i < rules.size(); i++) {
		ruleToModel(rules[i]);
	}
	return model;
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

// Identify the idObject returning the enum type
TypeObject parser::stringToType(string str) {
	if (!str.compare("cube"))
		return CUBE;
	else if (!str.compare("cylinder"))
		return CYLINDER;
	else if (!str.compare("scope"))
		return SCOPE;
	else if (!str.compare("plain"))
		return PLAIN;
	else if (!str.compare("sofa"))
		return SOFA;
	else if (!str.compare("table"))
		return TABLE;
	else if (!str.compare("cabinet"))
		return CABINET;
	else if (!str.compare("chair"))
		return CHAIR;
	else if (!str.compare("toy"))
		return TOY;
	else runtime_error("ERROR: stringToType cannot parse");
}

// Read a string line and transformed it into a rule
void parser::ruleToModel(rule r) {
	vector<string> keys = splitString(r.rules, ' ', ' '); 
	vector<modeler> *result;
	initModel();
	while (list.size()) {
		modeler currentmodel = list[0];
		list.erase(list.begin());

		string modelerName = currentmodel.getName();
		if (modelerName == r.head) {
			for (int i = 0; i < keys.size(); i++) {
				if (startsWith(keys[i], "T")) { // Translation
					vector<float> args = parseArguments(keys[i]);
					currentmodel.translate(vector3d(args[0], args[1], args[2]));
				}
				else if (startsWith(keys[i], "Subdiv")) { // Divide scope in smaller scopes (children)
					vector<float> args = parseArguments(keys[i]);
					vector<string> parameters = parseParameters(keys[i]);
					int axis = round(args[0]);
					vector<float> ratios;
					for (int i = 1; i < args.size(); ratios.push_back(args[i++])); // Parsing ratios
					vector<modeler> newmodelers = currentmodel.split(axis, ratios, parameters);
					copy(newmodelers.begin(), newmodelers.end(), back_inserter((*result))); // Save new modeler to result
				}
				else if (startsWith(keys[i], "S")) { // Set new size
					vector<float> args = parseArguments(keys[i]);
					currentmodel.setSize(vector3d(args[0], args[1], args[2]));
				}
				else if (startsWith(keys[i], "S3d")) { // Set new size for an specific axis
					vector<float> args = parseArguments(keys[i]);
					int axis = round(args[0]);
					vector3d size = currentmodel.getSize();
					size.setElement(axis, args[1]);
					currentmodel.setSize(size);
				}
				//else if (startsWith(keys[i], "I")) { // Instance of a figure
				//	TypeObject type = stringToType(splitString(keys[i], '(', ')')[1]);
				//	string texture = parseParameters(keys[i])[0]; // Texture
				//	currentmodel.setTexture(texture);
				//	currentmodel.setType(type);
				//	(*result).push_back(currentmodel);
				//}
				//else if (startsWith(keys[i], "Repeat")) { // Splitting scope into the same objects
				//	vector<float> args = parseArguments(keys[i]);
				//	string newmodelerName = parseParameters(keys[i])[0];
				//	int axis = round(args[0]);
				//	vector<modeler> newmodelers = currentmodel.repeat(r.head, axis, round(args[1]), newmodelerName);
				//	copy(newmodelers.begin(), newmodelers.end(), back_inserter((*result))); // Save new modeler to result
				//}
				//else if (startsWith(keys[i], "R")) { // Rotating around axis
				//	vector<float> args = parseArguments(keys[i]);
				//	currentmodel.rotate(vector3d(args[0], args[1], args[2]));
				//}
				//else if (startsWith(keys[i], "Comp")) { // Split scope into plains
				//	string type = splitString(keys[i], '(', ')')[1];
				//	vector<string> parameters = parseParameters(keys[i]);
				//	vector<modeler> newmodelers = currentmodel.componentSplit(type, parameters);
				//	copy(newmodelers.begin(), newmodelers.end(), back_inserter((result))); // Save new modeler to result
				//}
				else runtime_error("ERROR in parser : that command does not exist");
			}
		}
	}
}

// Split std::string in order to parse each part of the rule
vector<std::string> parser::splitString(std::string line, char del, char del2) {
	vector<std::string> wordsLine;
	unsigned int lastPos = 0;
	for (unsigned int i = 0; i < line.length(); i++) {
		if (line[i] == del || line[i] == del2) {
			wordsLine.push_back(line.substr(lastPos, i - lastPos));
			lastPos = i + 1;
		}
	}
	if (lastPos != line.length())
		wordsLine.push_back(line.substr(lastPos, line.length() - lastPos));
	return wordsLine;
}

// Compare words in order to find the specific prefix (e.g. T, R, Subdiv...)
bool parser::startsWith(std::string str, std::string prefix) {
	return str.compare(0, prefix.length(), prefix) == 0; // Return TRUE or FALSE
}

// Extract the arguments (relative and absolute values)
vector<float> parser::parseArguments(string token) {
	vector<string> args = splitString(splitString(token, '(', ')')[1], ',', ',');
	vector<float> vec;
	for (int k = 0; k < args.size(); k++) {
		if (startsWith(args[k], "rnd")) {
			vector<string> rArgs = splitString(splitString(args[k], '<', '>')[1], '-', '-');
			vec.push_back(stof(rArgs[0]));
			vec.push_back(stof(rArgs[1]));
		}
		else if (startsWith(args[k], "r")) {
			int axis = vec[0];
			string number = args[k].substr(1, args[k].length());
			modeler currentmodeler = modeler();
			float size = currentmodeler.getSize().getElement(axis);;
			vec.push_back(stof(number)*size);
		}
		else {
			double value = stof(args[k]);
			vec.push_back(value);
		}
	}
	return vec;
}

// Extract the arguments
vector<string> parser::parseParameters(string token) {
	return splitString(splitString(token, '{', '}')[1], ',', ',');
}
