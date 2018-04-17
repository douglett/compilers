#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <sstream>
using namespace std;

// parse
struct Prog {
	vector<string> cmdline;
	list<Prog> block;
};
int parsefile(const string& fname, Prog& prog);
int parseblock(const vector<string>& lines, int& pos, Prog& prog);
int parseline(const string& line, vector<string>& toklist);
// identifier functions
int is_alpha(char c);
int is_numeric(char c);
int is_strlit(const string& str);
int is_ident(const string& str);
string unstring(const string& v);
float to_num(const string& v);
int in_list(const string& s, const vector<string>& vs);
// run
struct ProgVM {
	Prog prog;
	vector<string> stack;
	map<string, string> env;
	string state;
	istream* input = &cin;
	int  run();
	int  runblock(const Prog& mprog);
	void reset();
	string getval(const string& v);
};


int main() {
	vector<vector<string>> scripts = { 
		{ "1", "" },
		{ "2", "blah" },
		{ "3", "fred" },
		{ "4", "5" },
		{ "5", "30" },
		{ "6", "7\nadd", "7\nmul" },
		{ "7", "" }
	};
	for (const auto& scr : scripts) {
		// parse vm
		ProgVM pvm;
		printf(":running file: %s.bas\n", scr[0].c_str());
		if (parsefile("scripts/"+scr[0]+".bas", pvm.prog))  continue;
		// run each test in sequence
		for (int i=1; i<scr.size(); i++) {
			pvm.reset();
			stringstream ss(scr[i]);
			if (ss.peek() != EOF)
				pvm.input = &ss;
			pvm.run();
		}
		printf("-----\n");
	}
}


int parsefile(const string& fname, Prog& prog) {
	// open
	fstream fs(fname, fstream::in);
	if (!fs.is_open())
		return fprintf(stderr, "error: could not open file: %s\n", fname.c_str()), 1;
	// load
	vector<string> lines;
	string s;
	while (getline(fs, s)) 
		lines.push_back(s);
	fs.close();
	// blockify
	prog = Prog{ .cmdline={"do"} };
	int pos = 0, err = 0;
	err = parseblock(lines, pos, prog);
	prog.block.push_back({ .cmdline={"end"} });
	return err;
}

int parseblock(const vector<string>& lines, int& pos, Prog& prog) {
	// if (prog.cmdline.size() == 0)  prog.cmdline = { "do" };
	vector<string> toklist;
	while (pos < lines.size()) {
		parseline( lines[pos++], toklist );
		prog.block.push_back({ .cmdline=toklist, .block={} });
		// sub lists
		if (toklist.size() == 0) ;
		// else if ( toklist[0] == "if" || toklist[0] == "ifn" ) {
		else if (in_list( toklist[0], { "do", "if", "ifn" } )) {
			int err = parseblock( lines, pos, prog.block.back() );
			if (err)  return err;
		}
		else if ( toklist[0] == "end" ) {
			break;
		}
	}
	return 0;
}

int parseline(const string& line, vector<string>& toklist) {
	toklist = {};
	string s;
	for (int i=0; i<line.size(); i++) {
		// space - next command
		if (isspace(line[i])) { 
			if (s.size()) toklist.push_back(s);  // save existing
			s = "";  // reset
		}
		// comment start - don't save
		else if (line[i] == '#') {
			break;
		}
		// string start - parse
		else if (line[i] == '"') {
			if (s.size()) toklist.push_back(s);  // save existing
			s = "\"";  // init
			for (++i; i<line.size(); i++) {
				s += line[i];
				if (line[i] == '"') { toklist.push_back(s);  s = "";  break; }
			}
		}
		// 
		else {
			s += line[i];
		}
	}
	// eol - save existing
	if (s.size())  toklist.push_back(s);
	return 0;
}


// identifier functions
int is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
int is_numeric(char c) {
	return (c >= '0' && c <= '9');
}
int is_strlit(const string& str) {
	return (str.size() >= 2 && str[0] == '"' && str.back() == '"');
}
int is_ident(const string& str) {
	if (str.size() == 0)  return 0;
	if (!is_alpha(str[0]))  return 0;
	for (char c : str)
		if (!is_alpha(c) && !is_numeric(c))  return 0;
	return 1;
}
int is_var(const string& v) {
	return ( is_strlit(v) || is_ident(v) );
}
string unstring(const string& v) {
	if (is_strlit(v))  return v.substr(1, v.length()-2);
	return v;
}
float to_num(const string& v) {
	stringstream ss( unstring(v) );
	float f = 0;
	ss >> f;
	return f;
}
int in_list(const string& s, const vector<string>& vs) {
	for (const auto& ss : vs)
		if (ss == s)  return 1;
	return 0;
}


// runable
int ProgVM::run() {
	return runblock( prog );
}
int ProgVM::runblock(const Prog& mprog) {
	const string BLOCKCMD = ( mprog.cmdline.size() ? mprog.cmdline[0] : "" );
	string s;
	int showcmd = 0;
	RESTART_BLOCK:
	for (const auto& block : mprog.block) {
		const auto& ln = block.cmdline;
		// show
		if (showcmd) {
			for (const auto& tok : ln)
				printf("[%s]", tok.c_str());
			printf("\n");
		}
		// parse
		if (ln.size() == 0) ;
		else if (ln[0] == "end")  ;  // noop
		else if (ln[0] == "let") {
			if ( ln.size() != 3 || !is_ident(ln[1]) || !is_var(ln[2]) )
				return fprintf(stderr, "error: let: expected identifier, value\n"), 1;
			env[ln[1]] = getval(ln[2]);
		}
		else if (in_list( ln[0], { "eq", "lt", "gt" } )) {
			if ( ln.size() != 3 || !is_var(ln[1]) || !is_var(ln[2]) )
				return fprintf(stderr, "error: %s: expected value, value\n", ln[0].c_str()), 1;
			string a = getval( ln[1] ), b = getval( ln[2] );
			int v = 0;
			// printf("%s  %s %s   %s %s\n", ln[0].c_str(), a.c_str(), b.c_str(), unstring(a).c_str(), unstring(b).c_str());
			if      (ln[0] == "eq" )  v = ( unstring(a) == unstring(b) );
			else if (ln[0] == "lt" )  v = ( to_num(a) < to_num(b) );
			else if (ln[0] == "gt" )  v = ( to_num(a) > to_num(b) );
			//else if (ln[0] == "or" )  v = ( unstring(a) == "1" || unstring(b) == "1" );
			stack.push_back('"' + to_string(v) + '"');
		}
		else if (in_list( ln[0], { "add", "sub", "mul", "div", "mod" } )) {
			if ( ln.size() != 3 || !is_ident(ln[1]) || !is_var(ln[2]) )
				return fprintf(stderr, "error: %s: expected identifier, value\n", ln[0].c_str()), 1;
			int a = to_num(getval( ln[1] )), b = to_num(getval( ln[2] ));
			if      (ln[0] == "add")  a += b;
			if      (ln[0] == "sub")  a -= b;
			else if (ln[0] == "mul")  a *= b;
			else if (ln[0] == "div")  a /= b;
			else if (ln[0] == "mod")  a %= b;
			s = '"' + to_string( a ) + '"';
			if (ln[1] == "pop")  stack.push_back( s );
			else  env[ln[1]] = s;
			
		}
		else if (ln[0] == "pop") {
			if (stack.size())  stack.pop_back();
		}
		else if (ln[0] == "push") {
			if ( ln.size() != 2 || !is_var(ln[1]) )
				return fprintf(stderr, "error: push: expected var\n"), 1;
			stack.push_back( getval( ln[1] ) );
		}
		else if (ln[0] == "do") {
			runblock( block );
		}
		else if (ln[0] == "continue") {
			state = "continue";
		}
		else if (ln[0] == "break") {
			state = "break";
		}
		else if (ln[0] == "if" || ln[0] == "ifn") {
			if ( ln.size() != 3 || !is_var(ln[1]) || ln[2] != "then" )
				return fprintf(stderr, "error: if: expected var, then\n"), 1;
			// printf("%s %s\n", ln[0].c_str(), stack.back().c_str());
			if ( ln[0] == "if"  && unstring(getval( ln[1] )) == "1" ) {
				// printf("ok\n"); 
				runblock( block );
			}
			if ( ln[0] == "ifn" && unstring(getval( ln[1] )) == "0" ) {
				runblock( block );
			}
			// else  printf("no\n");
		}
		else if (ln[0] == "print") {
			for (int i=1; i<ln.size(); i++) 
				printf("%s ", unstring(getval( ln[i] )).c_str() );
			printf("\n");
		}
		else if (ln[0] == "input") {
			if ( ln.size() != 2 || !is_ident(ln[1]) )
				return fprintf(stderr, "error: input: expected identifier\n"), 1;
			printf("> ");  // prompt
			getline(*input, s);
			env[ln[1]] = '"' + s + '"';
		}
		else {
			return fprintf(stderr, "syntax error: unknown command %s\n", ln[0].c_str()), 1;
		}
		// -----
		// state changes
		if (showcmd && state.size())  printf("STATE: continue. cmd: %s\n", BLOCKCMD.c_str());
		if (state == "continue") {
			if (BLOCKCMD == "do") { state = ""; goto RESTART_BLOCK; }
			else  break;
		}
		else if (state == "break") {
			if (BLOCKCMD == "do")  state = ""; 
			break;
		}
	}
	return 0;
}

void ProgVM::reset() {
	stack = {};
	env = {};
	state = "";
	input = &cin;
}

string ProgVM::getval(const string& v) {
	if (v == "pop"  && stack.size()) { string s = stack.back();  stack.pop_back();  return s; }
	if (v == "peek" && stack.size()) { return stack.back(); }
	if (is_strlit(v))  return v;
	if (env.count(v))  return env[v];
	return "\"undefined\"";
}
