#include "dbparse.h"
#include <map>
#include <cassert>

using namespace std;


static vector<string> prog;
static map<string, vector<i32>> env;
static i32 PC = 0;


static vector<string> split(const string& s) {
	vector<string> vs={""};
	for (char c : s)
		if (isspace(c)) 
			{ if (vs.back().length())  vs.push_back(""); }
		else 
			{ vs.back() += c; }
	if (vs.size() > 1 && vs.back().length() == 0)
		vs.pop_back();
	return vs;
}

static i32 match_end(int pos) {
	assert(pos >= 0 && pos < prog.size());
	int indent = 0;
	while (pos < prog.size()) {
		auto cmd = split(prog[pos])[0];
		if (cmd == "WHILE" || cmd == "IF") 
			{ indent++; }
		else if (cmd == "END")
			{ if (indent == 0)  break;  else  indent--; }
		pos++;
	}
	return pos;
}

int r_runprog(const std::vector<std::string>& program) {
	prog = program;
	PC = 0;
	
	while (PC < prog.size()) {
		//printf("%s\n", prog[PC].c_str());
		auto cmd = split(prog[PC]);
		printf("%s\n", cmd[0].c_str());
		PC++;
	}
	return 0;
}
