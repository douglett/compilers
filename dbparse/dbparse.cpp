#include "parsers/pgeneral.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;


// static stuff
struct Token {
	string val;
	string type; };
static vector<vector<Token>> lines;
static int lineno = 0;


// helpers
static int in_list(const string& k, const vector<string>& vs) {
	for (const auto& s : vs)
		if (s == k)  return 1;
	return 0;
}
static Token identifytok(const string& s) {
	Token tok = { .val=s, .type="???" };
	if (in_list(s, { "end", "if" }))
		tok.type = "cmd";
	else if (pgeneral::is_ident(s))
		tok.type = "ident";
	return tok;
}


// basic checks
static void p_check_eol(int pos) {
	if (pos < lines[lineno].size()-1)
		throw (string) "expected eol as token pos: " + to_string(pos);
}
static void p_check_empty() {
	if (lineno >= lines.size())
		throw (string) "expected line, found eof";
	if (lines[lineno].size() == 0)
		throw (string) "expected line contents, found empty line";
}
// parse if statement
static void p_if() {
	p_check_empty();
	const auto& ln = lines[lineno];
	if (ln.size() <= 1)
		throw (string) "expected expression after if";
	if (ln[1].type != "bracket" && ln[1].val != "(")
		throw (string) "expected bracketed expression after if";
	// save
	printf("IF  [expr]\n");
}
// parse dim statements
static void p_dim() {
	p_check_empty();
	const auto& ln = lines[lineno];
	if (ln.size() <= 1)
		throw (string) "expected ident after dim, got eol";
	if (ln[1].type != "ident")
		throw (string) "expected ident after dim, got: " + ln[1].type +":"+ ln[1].val;
	// save
	printf("DIM [%s]\n", ln[1].val.c_str());
}
// parse each item in a block
static void p_block() {
	while (lineno < lines.size()) {
		// check if line is empty
		if (lines[lineno].size() == 0)
			{ lineno++;  continue; }
		const auto& cmd = lines[lineno][0];  // get command
		// end - break block
		if (cmd.val == "end") { 
			p_check_eol(1);
			lineno++;
			return;
		}
		else if (cmd.val == "if") {
			p_if();
			lineno++;
		}
		else if (cmd.val == "dim") {
			p_dim();
			lineno++;
		}
		// unknown - break
		else {
			throw (string) "unknown command in block: " + cmd.val;
		}
	}
}


int pfile(const string& fname) {
	printf("parsing file: %s\n", fname.c_str());
	// reset
	lines = {}, lineno = 0;
	pgeneral::punclist = "()[]";
	// open and save tokens
	fstream fs(fname, fstream::in);
	string s;
	while (getline(fs, s)) {
		auto vs = pgeneral::tokenize(s);
		lines.push_back({});
		for (const auto& s : vs)
			lines.back().push_back(identifytok(s));
	}
	// parse
	try {
		p_block();
		return 0;
	}
	catch (const string& err) {
		fprintf(stderr, "error: %s\n", err.c_str());
		return 1;
	}
}


int main() {
	pfile("test.bas");
}