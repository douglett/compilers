#include "globals.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include <map>
using namespace std;


typedef int32_t i32;
struct Node {
	string v;
	vector<Node> c;
};
Node prog = {
	"prog", {
		{"function main", {
			{"print hello world", {}},
			{"let $a some_data_by_doug blah blah", {}},
			{"print got: [$a]", {}},
			{"call test"},
			{"rem call inputtest"},
			{"call test2"}
		}},
		{"function test", {
			{"rem testing function", {}},
			{"print test function in running", {}},
			{"let $a 1", {}},
			{"if eq $a 1", {
				{"print a equals 1!", {}}
			}},
			{"if lt $a 1", {
				{"print $a is less than 10", {}}
			}}
		}},
		{"function inputtest", {
			{"let $b", {}},
			{"input $b", {}},
			{"print you input: [$b]", {}}
		}},
		{"function test2", {
			{"let $b hello"},
			{"let $b $b$b"},
			{"print [$b]"},
			{"let $b 1"},
			{"math + $b 20"},
			{"print $b"}
		}}
	}
};
map<string,string> env;


// string command (first item in string)
string cmdtype(const string& str) {
	stringstream ss(str);
	string s;
	ss >> s;
	return s;
}
// string value (all items after first)
string cmdval(const string& str) {
	stringstream ss(str);
	string s;
	ss >> s >> ws;   // remove first command & leading whitespace
	getline(ss, s);  // get rest of command
	return s;
}
// return type and val from string
pair<string, string> cmdat(const string& str, i32 pos) {
	stringstream ss(str);
	string t, v;
	for (int i=0; i<=pos; i++)
		ss >> t >> ws;
	getline(ss, v);
	return { t, v };
}
string cmdtype(const Node& n) {
	return cmdtype(n.v);
}
string cmdval(const Node& n) {
	return cmdval(n.v);
}

// string to int
i32 toint(const string& s) {
	stringstream ss(s);
	i32 i = 0;
	ss >> i;
	return i;
}

// check if string is valid id
void checkid(const string& id) {
	if (id.size() >= 2 && id[0] == '$') {
		for (int i=1; i<id.size(); i++)
			if (id[i] >= 'a' && id[i] <= 'z') ;
			else if (i >= 2 && id[i] >= '0' && id[i] <= '9') ;
			else  throw (string) "expected identifier: "+id;
	}			
}

// find function by id
const Node& getfunc(const string& name) {
	for (const Node& nn : prog.c)
		if (cmdval(nn) == name)
			return nn;
	throw (string) "could not find function: " + name;
}
// get variable by $id
string& getmem(const string& id, int let=0) {
	checkid(id);
	if (!let && env.count(id) == 0)
		throw (string) "undefined identifier: "+id;
	return env[id];
}
// replace $id with var contents
string injectvars(const string& str) {
	for (const auto& e : env) {
		int pos = str.find(e.first);
		if (pos != string::npos) {
			string s1 = injectvars(str.substr(0, pos));  // recursive replace
			string s2 = injectvars(str.substr(pos + e.first.length()));
			return s1 + e.second + s2;  // return new string
		}
	}
	return str;  // no match
}
// check for true / false
int testval(const string& val) {
	#define cmdtype2(CSTR) injectvars(cmdtype(cmdval(CSTR)))
	#define cmdtype3(CSTR) cmdtype2(cmdval(val))
	if (cmdtype(val) == "eq"  )  return cmdtype2(val) == cmdtype3(val);
	if (cmdtype(val) == "not" )  return cmdtype2(val) != cmdtype3(val);
	if (cmdtype(val) == "lt"  )  return toint(cmdtype2(val)) <  toint(cmdtype3(val));
	if (cmdtype(val) == "gt"  )  return toint(cmdtype2(val)) >  toint(cmdtype3(val));
	if (cmdtype(val) == "lte" )  return toint(cmdtype2(val)) <= toint(cmdtype3(val));
	if (cmdtype(val) == "gte" )  return toint(cmdtype2(val)) >= toint(cmdtype3(val));
	return val.length() > 0;
}

void runn(const Node& n) {
	for (int i=0; i<n.c.size(); i++) {
		auto& nn = n.c[i];
		// printf("[%s]\n", nn.v.c_str());
		if (cmdtype(nn) == "" || cmdtype(nn) == "rem")
			continue;  // noop
		else if (cmdtype(nn) == "function")
			{ if (n.v != "prog")  throw (string) "function not on top scope: " + nn.v; }
		else if (cmdtype(nn) == "print")
			printf(": %s\n", injectvars(cmdval(nn)).c_str());
		else if (cmdtype(nn) == "input")
			printf("> "), getline( cin, getmem(cmdtype(cmdval(nn))) );
		else if (cmdtype(nn) == "let")
			getmem(cmdtype(cmdval(nn)), 1) = injectvars(cmdval(cmdval(nn)));
		else if (cmdtype(nn) == "call")
			runn(getfunc(cmdval(nn)));
		else if (cmdtype(nn) == "if")
			{ if (testval(cmdval(nn)))  runn(nn); }
		else
			throw (string) "unknown command line: ["+nn.v+"]";
	}
}

int main() {
	printf("start\n");
	try {
		printf("<<run prog>>\n");
		runn(prog);
		printf("<<run main>>\n");
		runn(getfunc("main"));
		return 0;
	}
	catch (const string& err) {
		fprintf(stderr, "error: %s\n", err.c_str());
		return 1;
	}
}