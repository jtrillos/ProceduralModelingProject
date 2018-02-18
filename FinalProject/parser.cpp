#include <iterator>
#include <algorithm> 
#include <stack>
#include <string>
#include <fstream>
#include "parser.h"

using namespace std;

// Random number generator objects (Non-deterministic random number generator)
random_device rd;
// A Mersenne Twister pseudo-random generator of 32-bit numbers
mt19937 mt(rd());
// Stack -> FILO data structure
stack<modeler> *processing = new stack<modeler>(); // Modeler being processed

// Import the file (that contains the rules) in the attribute fileName
// Constructor
parser::parser(string file) {
	this->fileName = file;
}

// Reads the file and extract the lines saving them in a vector
vector<string> parser::readLines(string file) {
	vector<string> linesFile;
	ifstream dict_file(file);
	string line;
	while (getline(dict_file, line))
		if (line[0] != '#') // Ignoring '#' that are use for comments
			linesFile.push_back(line);
	return linesFile;
}

// Split string in order to parse each part of the rule
vector<string> parser::splitString(string line, char del, char del2) {
	vector<string> wordsLine;
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
bool parser::startsWith(string str, string prefix) {
	return str.compare(0, prefix.length(), prefix) == 0; // Return TRUE or FALSE
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
function<vector<modeler>(modeler)> parser::stringToRule(string str) {
	auto keys = splitString(str, ' ', ' '); // Operations to be performed
	string sourceScope = keys[0]; // Modeler the rule will be applied (head of the rule)
	vector<modeler> *result = new vector<modeler>(); // Modeler being return to build the tree

	function<void(modeler)> rule = [=](modeler x) { (*processing).push(x); };  // Initial rule using lambdas (unnamed function)

	for (int i = 2; i < keys.size(); i++) { // Starts in i = 2, after '=>'

		if (startsWith(keys[i], "T")) { // Translation
			auto args = parseArguments(keys[i]);
			rule = [=](modeler x) {
				rule(x);
				modeler& currentmodeler = (*processing).top();
				currentmodeler.translate(vector3d(args[0](), args[1](), args[2]()));
			};
		}

		else if (startsWith(keys[i], "Subdiv")) { // Divide scope in smaller scopes (children)
			auto args = parseArguments(keys[i]);
			auto parameters = parseParameters(keys[i]);
			int axis = round(args[0]());
			rule = [=](modeler x) {
				rule(x);
				modeler& currentmodeler = (*processing).top();
				vector<float> ratios;
				for (int i = 1; i < args.size(); ratios.push_back(args[i++]())); // Parsing ratios
				auto newmodelers = currentmodeler.split(axis, ratios, parameters);
				copy(newmodelers.begin(), newmodelers.end(), back_inserter((*result))); // Save new modeler to result
			};
		}

		else if (startsWith(keys[i], "S")) { // Set new size
			auto args = parseArguments(keys[i]);
			rule = [=](modeler x) {
				rule(x);
				modeler& currentmodeler = (*processing).top();
				currentmodeler.setSize(vector3d(args[0](), args[1](), args[2]()));
			};
		}

		else if (startsWith(keys[i], "[")) { // Push current scope to the stack
			rule = [=](modeler x) {
				rule(x);
				(*processing).push(x);
				(*processing).push(modeler(x.getName(), x.getScopePosition(), x.getSize(), x.getType()));
			};
		}

		else if (startsWith(keys[i], "]")) { // Pop current scope from the stack
			rule = [=](modeler x) {
				rule(x);
				(*processing).pop();
			};
		}

		else if (startsWith(keys[i], "I")) { // Instance of a figure
			auto type = stringToType(splitString(keys[i], '(', ')')[1]);
			auto material = parseParameters(keys[i])[0]; // Texture
			rule = [=](modeler x) {
				rule(x);
				modeler& currentmodeler = (*processing).top();
				currentmodeler.setName(material);
				currentmodeler.setType(type);
				(*result).push_back(currentmodeler);
			};
		}

		else if (startsWith(keys[i], "Repeat")) { // Splitting scope into the same objects
			auto args = parseArguments(keys[i]);
			auto newmodelerName = parseParameters(keys[i])[0];
			int axis = round(args[0]());
			rule = [=](modeler x) {
				rule(x);
				modeler& currentmodeler = (*processing).top();
				auto newmodelers = currentmodeler.repeat(axis, round(args[1]()), newmodelerName);
				copy(newmodelers.begin(), newmodelers.end(), back_inserter((*result))); // Save new modeler to result
			};
		}
		else if (startsWith(keys[i], "R")) { // Rotating around axis
			auto args = parseArguments(keys[i]);
			rule = [=](modeler x) {
				rule(x);
				modeler& currentmodeler = (*processing).top();
				currentmodeler.rotate(vector3d(args[0](), args[1](), args[2]()));
			};
		}

		else if (startsWith(keys[i], "Comp")) { // Split scope into plains
			auto type = splitString(keys[i], '(', ')')[1];
			auto parameters = parseParameters(keys[i]);
			rule = [=](modeler x) {
				rule(x);
				modeler& currentmodeler = (*processing).top();
				auto newmodelers = currentmodeler.componentSplit(type, parameters);
				copy(newmodelers.begin(), newmodelers.end(), back_inserter((*result))); // Save new modeler to result
			};
		}
		else runtime_error("ERROR in parser : that command does not exist");
	}

	return [=](modeler modeler) {
		(*result).clear();
		if (sourceScope.compare(modeler.getName()) == 0) // Apply rule if name in rule head matches
			rule(modeler);
		return (*result); 
	};
}

// Extract the arguments (relative and absolute values)
vector<function<float()>> parser::parseArguments(string token) {
	auto args = splitString(splitString(token, '(', ')')[1], ',', ',');
	vector<function<float()>> funs;
	for (int k = 0; k < args.size(); k++) {
		if (startsWith(args[k], "rnd")) {
			auto rArgs = splitString(splitString(args[k], '<', '>')[1], '-', '-');
			uniform_real_distribution<double> dist(stof(rArgs[0]), stof(rArgs[1]));
			funs.push_back([=] { return dist(mt); });
		}
		else if (startsWith(args[k], "r")) {
			int axis = funs[0]();
			string number = args[k].substr(1, args[k].length());
			funs.push_back([=] {
				modeler& currentmodeler = (*processing).top();
				float size = currentmodeler.getSize().getElement(axis);;
				return stof(number)*size; });
		}
		else {
			float value = stof(args[k]);
			funs.push_back([=] { return value; });
		}
	}
	return funs;
}

// Extract the arguments
vector<string> parser::parseParameters(string token) {
	return splitString(splitString(token, '{', '}')[1], ',', ',');
}

// Extract the information from the file and converted in a set of rules
vector<function<vector<modeler>(modeler)>> parser::parseRules() {
	vector<function<vector<modeler>(modeler)>> rules;
	auto lines = readLines(this->fileName);
	for (auto line : lines)
		rules.push_back(stringToRule(line));
	return rules;
}
