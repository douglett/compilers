#include "dbparse.h"
#include "parsers/pgeneral.h"
#include <map>
#include <cassert>
using namespace std;

// vars
static vector<string> prog;
static map<string, vector<i32>> env;
static i32 PC = 0;
static vector<pair<string, i32>> bstack;

// helpers
static vector<string> split(const string& s) {
	vector<string> vs={""};
	for (char c : s)
		if   (isspace(c)) { if (vs.back().length())  vs.push_back(""); }  // new entry
		else { vs.back() += c; }
	if (vs.size() > 1 && vs.back().length() == 0)  vs.pop_back();  // return at least one entry
	return vs;
}

// find 'end' matching current block
static i32 match_end(int pos) {
	assert(pos >= 0 && pos < prog.size());
	int indent = 0;
	while (pos < prog.size()) {
		auto cmd = split(prog[pos])[0];
		if (cmd == "WHILE" || cmd == "IF") { indent++; }
		else if (cmd == "END") { if (indent == 0)  break;  else  indent--; }
		pos++;
	}
	return pos;
}
static i32 match_else(int pos) {
	assert(pos >= 0 && pos < prog.size());
	int indent = 0;
	while (pos < prog.size()) {
		auto cmd = split(prog[pos])[0];
		if (cmd == "WHILE" || cmd == "IF") { indent++; }
		else if (cmd == "ELIF" || cmd == "ELSE" || cmd == "END") 
			{ if (indent == 0)  break;  else  indent--; }
		pos++;
	}
	return pos;
}

static i32 getval(const string& id) {
	if (id == "_top") {  // stack top
		if (env["_top"].size() == 0)  throw (string) "math error: _top underflow";
		i32 i = env["_top"].back();  env["_top"].pop_back();  return i; }
	if (pgeneral::is_number(id))  return pgeneral::strtonum(id);  // number
	if (env.count(id))  return env[id][0];  // env variable
	throw (string) "unknown value type: " + id;
}
static void math(const string& OP, const string& id1, const string& id2) {
	i32 i2 = getval(id2);  // get 2nd opcode first (in case of stack usage)
	i32 i1 = getval(id1);
	if      (OP == "==")  env["_top"].push_back( i1 == i2 );
	else if (OP == ">" )  env["_top"].push_back( i1 >  i2 );
	else if (OP == "<" )  env["_top"].push_back( i1 <  i2 );
	else if (OP == ">=")  env["_top"].push_back( i1 >= i2 );
	else if (OP == "<=")  env["_top"].push_back( i1 <= i2 );
	else if (OP == "+" )  env["_top"].push_back( i1 +  i2 );
	else if (OP == "-" )  env["_top"].push_back( i1 -  i2 );
	else if (OP == "*" )  env["_top"].push_back( i1 *  i2 );
	else if (OP == "/" )  env["_top"].push_back( i1 /  i2 );
	else  throw (string) "unknown math operation: " + OP;
}


int r_runprog(const std::vector<std::string>& program) {
	try {
		// reset
		prog = program;
		env = {{ "_top", {} }};
		PC = 0;
		// loop
		while (PC < prog.size()) {
			auto cmd = split(prog[PC]);
			printf("% 3d  %s\n", PC, cmd[0].c_str());
			if (cmd[0] == "DIM")
				env[cmd[1]] = vector<i32>( pgeneral::strtonum(cmd[2]), 0 );
			else if (cmd[0].substr(0,2) == "OP")
				math( cmd[0].substr(2), cmd[1], cmd[2] );
			else if (cmd[0] == "IF")
				{ if (!getval("_top"))  PC = match_else(PC+1); }
			else if (cmd[0] == "ELIF")
				{ if (!getval("_top"))  PC = match_else(PC+1),  bstack.pop_back(); }
			else if (cmd[0] == "ELSE")
				{ }
			else if (cmd[0] == "END") {
				if (bstack.back().first == "WHILE") {
					PC = bstack.back().second;
					bstack.pop_back();
					continue; }
				else {
					bstack.pop_back(); }
			}
			else
				throw (string) "unknown command: " + cmd[0];
			PC++;
		}
		// 
		// printf("%d  %d\n", 12, match_end(12));
		// printf("%d  %d\n", 22, match_end(22));
		return 0;
	}
	catch (const string& err) {
		fprintf(stderr, "error [%d]: %s\n", PC, err.c_str());
		return 1;
	}
}
