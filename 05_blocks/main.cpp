#include "globals.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
using namespace std;

struct Node {
	string v;
	vector<Node> c;
};

Node prog = {
	"prog", {
		{"function main", {
			{"print hello world", {}},
			{"let $a some_data_by_doug", {}},
			{"print got: $a", {}}
		}}
	}
};

map<string,string> env;

string cmdtype(const string& str) {
	stringstream ss(str);
	string s;
	ss >> s;
	return s;
}
string cmdtype(const Node& n) {
	return cmdtype(n.v);
}
string cmdval(const string& str) {
	stringstream ss(str);
	string s;
	ss >> s >> ws;   // remove first command & leading whitespace
	getline(ss, s);  // get rest of command
	return s;
}
string cmdval(const Node& n) {
	return cmdval(n.v);
}
void checkid(const string& id) {
	if (id.size() >= 2 && id[0] == '$') {
		for (int i=1; i<id.size(); i++)
			if (id[i] >= 'a' && id[i] <= 'z') ;
			else if (i >= 2 && id[i] >= '0' && id[i] <= '9') ;
			else  throw (string) "expected identifier: "+id;
	}			
}

const Node& getfunc(const string& name) {
	for (const Node& nn : prog.c)
		if (cmdval(nn) == name)
			return nn;
	throw (string) "could not find function: " + name;
}
string& getmem(const string& id, int let=0) {
	checkid(id);
	if (!let && env.count(id) == 0)
		throw (string) "undefined identifier: "+id;
	return env[id];
}
string injectvars(string str) {
	return str;
}

void runn(const Node& n) {
	for (int i=0; i<n.c.size(); i++) {
		auto& nn = n.c[i];
		// printf("[%s]\n", nn.v.c_str());
		if (cmdtype(nn) == "function")
			;
		else if (cmdtype(nn) == "print")
			printf("> %s\n", injectvars(cmdval(nn)).c_str());
		else if (cmdtype(nn) == "let")
			getmem(cmdtype(cmdval(nn)), 1) = injectvars(cmdval(cmdval(nn)));
		else
			throw (string) "unknown command line: ["+nn.v+"]";
	}
}

int main() {
	printf("start\n");
	try {
		printf("<<parse>>\n");
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