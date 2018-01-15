#include "dbparse.h"
using namespace std;

static vector<string> prog;
static i32 PC = 0;

static vector<string> split(const string& s) {
	vector<string> vs={""};
	for (char c : s)
		if (isspace(c)) 
			{ if (vs.back().length())  vs.push_back(""); }
		else 
			{ vs.back() += c; }
	if (vs.back().length() == 0)
		vs.pop_back();
	return vs;
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