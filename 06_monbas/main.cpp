#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <fstream>
#include <sstream>
using namespace std;

struct Prog {
	vector<string> cmdline;
	list<Prog> block;
};
struct ProgVM {
	Prog prog;
	vector<string> stack;
	map<string, string> env;
	istream* input = &cin;
	int run();
	int runblock(const Prog& mprog);
	string getval(const string& v);
};
int parsefile(const string& fname, Prog& prog);
int parseblock(const vector<string>& lines, int& pos, Prog& prog);
int parseline(const string& line, vector<string>& toklist);
// identifier functions
int is_alpha(char c);
int is_numeric(char c);
int is_strlit(const string& str);
int is_ident(const string& str);
// run
int runprog(const Prog& prog, istream& input);

int main() {
	vector<string> scripts = { "1", "2", "3" };
	for (const auto& s : scripts) {
		ProgVM pvm;
		printf(":running file: %s.bas\n", s.c_str());
		if (parsefile("scripts/"+s+".bas", pvm.prog))  continue;
		if (s == "2")
			{ stringstream ss("blah");  pvm.input = &ss;  pvm.run(); }
		else if (s == "3")
			{ stringstream ss("fred");  pvm.input = &ss;  pvm.run(); }
		else
			{ pvm.run(); }
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
	prog = Prog{};
	int pos = 0;
	return parseblock(lines, pos, prog);
}

int parseblock(const vector<string>& lines, int& pos, Prog& prog) {
	if (prog.cmdline.size() == 0)  prog.cmdline = { "do" };
	vector<string> toklist;
	while (pos < lines.size()) {
		parseline( lines[pos++], toklist );
		prog.block.push_back({ .cmdline=toklist, .block={} });
		// sub lists
		if (toklist.size() == 0) ;
		else if ( toklist[0] == "if" || toklist[0] == "ifn" ) {
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
string unstrint(const string& v) {
	if (is_strlit(v))  return v.substr(1, v.length()-1);
	return v;
}


// runable
int ProgVM::run() {
	return runblock( prog );
}
int ProgVM::runblock(const Prog& mprog) {
	string s;
	int showcmd = 0;
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
		else if (ln[0] == "eq") {
			if ( ln.size() != 3 || !is_ident(ln[1]) || !is_var(ln[2]) )
				return fprintf(stderr, "error: eq: expected identifier, value\n"), 1;
			// printf("%s %s\n",  getval(ln[1]).c_str(), getval(ln[2]).c_str());
			stack.push_back(to_string( getval(ln[1]) == getval(ln[2]) ));
		}
		else if (ln[0] == "pop") {
			if (stack.size())  stack.pop_back();
		}
		else if (ln[0] == "if" || ln[0] == "ifn") {
			if ( ln.size() != 3 || !is_var(ln[1]) || ln[2] != "then" )
				return fprintf(stderr, "error: if: expected var, then\n"), 1;
			// printf("%s %s\n", ln[0].c_str(), stack.back().c_str());
			if ( ln[0] == "if" && getval(ln[1]) == "1" ) {
				// printf("ok\n"); 
				runblock(block);
			}
			if ( ln[0] == "ifn" && getval(ln[1]) == "0" ) {
				runblock(block);
			}
			// else  printf("no\n");
		}
		else if (ln[0] == "print") {
			for (int i=1; i<ln.size(); i++) 
				printf("%s ", getval(ln[i]).c_str());
			printf("\n");
		}
		else if (ln[0] == "input") {
			if ( ln.size() != 2 || !is_ident(ln[1]) )
				return fprintf(stderr, "error: input: expected identifier\n"), 1;
			getline(*input, s);
			env[ln[1]] = '"' + s + '"';
		}
		else {
			return fprintf(stderr, "error: unknown command %s\n", ln[0].c_str()), 1;
		}
	}
	return 0;
}

string ProgVM::getval(const string& v) {
	if (v == "pop"  && stack.size()) { string s = stack.back();  stack.pop_back();  return s; }
	if (v == "peek" && stack.size()) { return stack.back(); }
	if (is_strlit(v))  return v;
	if (env.count(v))  return env[v];
	return "\"undefined\"";
}

// int rungetval(const string& s) {
	
// }