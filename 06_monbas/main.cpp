#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
using namespace std;

struct a {
	vector<string> ln;
	vector<a> children;
};
typedef  vector<vector<string>>  Prog;
int parsefile(const string& fname, Prog& prog);
int runprog(const Prog& prog);
int is_alpha(char c);
int is_numeric(char c);
int is_ident(const string& str);
int is_strlit(const string& str);

int main() {
	vector<string> scripts = { "1", "2", "3" };
	Prog prog;
	for (const auto& s : scripts) {
		printf(":running file: %s.bas\n", s.c_str());
		if (parsefile("scripts/"+s+".bas", prog))  continue;
		runprog(prog);
		printf("-----\n");
	}
}


int parsefile(const string& fname, Prog& prog) {
	// load
	fstream fs(fname, fstream::in);
	if (!fs.is_open())
		return fprintf(stderr, "error: could not open file: %s\n", fname.c_str()), 1;
	// reset
	prog = {};
	string line;
	while (getline(fs, line)) {
		prog.push_back({});
		string s;
		for (int i=0; i<line.size(); i++) {
			// space - next command
			if (isspace(line[i])) { 
				if (s.size()) prog.back().push_back(s);  // save existing
				s = "";  // reset
			}
			// string start - parse
			else if (line[i] == '"') {
				if (s.size()) prog.back().push_back(s);  // save existing
				s = "\"";  // init
				for (++i; i<line.size(); i++) {
					s += line[i];
					if (line[i] == '"') { prog.back().push_back(s);  s = "";  break; }
				}
			}
			// 
			else {
				s += line[i];
			}
		}
		// eol - save existing
		if (s.size())  prog.back().push_back(s);
	}
	return 0;
}

int runprog(const Prog& prog) {
	map<string, string> env;
	string s;
	int showcmd = 0;
	for (const auto& ln : prog) {
		// show
		if (showcmd) {
			for (const auto& tok : ln)
				printf("[%s]", tok.c_str());
			printf("\n");
		}
		// parse
		if (ln.size() == 0) ;
		else if (ln[0] == "print") {
			for (int i=1; i<ln.size(); i++) {
				if      ( is_strlit(ln[i]) )  s = ln[i];
				else if ( is_ident(ln[i])  )  s = env[ln[i]];
				printf("%s ", s.c_str());
			}
			printf("\n");
		}
		else if (ln[0] == "input") {
			if (ln.size() != 2 || !is_ident(ln[1]))
				return fprintf(stderr, "error: expected identifier\n"), 1;
			getline(cin, s);
			env[ln[1]] = '"' + s + '"';
		}
		else {
			return fprintf(stderr, "error: unknown command %s\n", ln[0].c_str()), 1;
		}
	}
	return 0;
}

int is_alpha(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

int is_numeric(char c) {
	return (c >= '0' && c <= '9');
}

int is_ident(const string& str) {
	if (str.size() == 0)  return 0;
	if (!is_alpha(str[0]))  return 0;
	for (char c : str)
		if (!is_alpha(c) && !is_numeric(c))  return 0;
	return 1;
}

int is_strlit(const string& str) {
	return (str.size() >= 2 && str[0] == '"' && str.back() == '"');
}