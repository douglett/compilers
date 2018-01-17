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

// get value from id or number string
static i32 getval(const string& id) {
	if (id == "_top") {  // stack top
		if (env["_top"].size() == 0)  throw (string) "math error: _top underflow";
		i32 i = env["_top"].back();  env["_top"].pop_back();  return i; }
	if (pgeneral::is_number(id))  return pgeneral::strtonum(id);  // number
	if (env.count(id))  return env[id][0];  // env variable
	throw (string) "unknown value type: " + id;
}
// match current item to end
static i32 match_end() {
	auto cmd = split( prog[PC] );  // command list
	int pos = pgeneral::strtonum( cmd.back() );
	return pos;
}
// do math operation
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
	printf("math: %d %s %d = %d\n", i1, OP.c_str(), i2, env["_top"][0]);
}
// mainloop
static int mainloop() {
	while (PC < prog.size()) {
		auto cmd = split(prog[PC]);  // command list
		printf("% 3d  %s\n", PC, cmd[0].c_str());  // show
		// select first command
		if (cmd[0] == "EOF")
			break;
		else if (cmd[0] == "DIM")
			env[cmd[1]] = vector<i32>( pgeneral::strtonum(cmd[2]), 0 );
		else if (cmd[0] == "SET")
			env[cmd[1]][0] = getval(cmd[2]);
		else if (cmd[0].substr(0,2) == "OP")
			math( cmd[0].substr(2), cmd[1], cmd[2] );
		else if (cmd[0] == "IF")
			{ if (!getval(cmd[1]))  PC = match_end();  continue; }
		else if (cmd[0] == "ELIF")
			{ if (!getval(cmd[1]))  PC = match_end();  continue; }
		else if (cmd[0] == "WHILE")
			{ if (!getval(cmd[1]))  PC = match_end();  continue; }
		
		else if (in_list(cmd[0], { "ELSE", "END", "LABEL", "PRINT", "BREAK" }))
			;  // NOOP
		// unknown command
		else
			throw (string) "unknown command: " + cmd[0];
		PC++;
	}
	return 0;
}


int r_runprog(const std::vector<std::string>& program) {
	try {
		// reset
		prog = program;
		env = {{ "_top", {} }};
		PC = 0;
		// run
		return mainloop();
	}
	catch (const string& err) {
		fprintf(stderr, "error [%d]: %s\n", PC, err.c_str());
		return 1;
	}
}
