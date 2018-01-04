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
	if (s.size() == 0)
		;  // shouldn't happen
	else if (in_list(s, { "end", "if", "elif", "while", "goto", "break", "dim", "print" }))
		tok.type = "cmd";
	else if (in_list(s, { "=", "==", "!=", "<", ">", "<=", ">=" "+", "-", "*", "/", "&&", "||" }))
		tok.type = "oper";
	else if (in_list(s, { "(", ")", "[", "]" }))
		tok.type = "bracket";
	else if (pgeneral::is_number(s))
		tok.type = "num";
	else if (pgeneral::is_ident(s))
		tok.type = "ident";
	else if (pgeneral::is_strlit(s))
		tok.type = "str";
	else if (s.back() == '$' && pgeneral::is_ident(s.substr(0, s.length()-1)))
		tok.type = "ident_asstring";
	else if (s.back() == '@' && pgeneral::is_ident(s.substr(0, s.length()-1)))
		tok.type = "ident_asarray";
	else if (s.back() == ':' && pgeneral::is_ident(s.substr(0, s.length()-1)))
		tok.type = "label";
	return tok;
}
static string tokstr(const Token& t) {
	return t.val + ":" + t.type;
}


// basic checks
static void p_expect_eol(int pos) {
	// printf(":: %d %d\n", pos, lines[lineno].size()-1);
	if (pos != lines[lineno].size())
		throw (string) "expected eol at token pos: " + to_string(pos);
}
static void p_expect_next(int pos) {
	if (pos >= lines[lineno].size())	
		throw (string) "unexpected eol at token pos: " + to_string(pos);
}
static void p_expect_nonempty() {
	if (lineno >= lines.size())
		throw (string) "expected line, found eof";
	if (lines[lineno].size() == 0)
		throw (string) "expected line contents, found empty line";
}
// parse end
static void p_end() {
	p_expect_nonempty();
	p_expect_eol(1);
	// save
	printf("END\n");
}
// parse if or elif statement
static void p_if() {
	p_expect_nonempty();
	const auto& ln = lines[lineno];
	const string& type = ln[0].val;
	p_expect_next(1);
	if (ln[1].type != "bracket" || ln[1].val != "(")
		throw (string) "expected bracketed expression after " + type;
	p_expect_next(2);
	if (ln.back().type != "bracket" || ln.back().val != ")")
		throw (string) "expected close bracket after " + type;
	// save
	if (type == "if")
		printf("IF     [expr]\n");
	else 
		printf("ELIF   [expr]\n");
}
// parse dim statements
static void p_dim() {
	p_expect_nonempty();
	const auto& ln = lines[lineno];
	if (ln.size() <= 1)
		throw (string) "expected ident after dim, got eol";
	if (ln[1].type != "ident")
		throw (string) "expected ident after dim, got: " + tokstr(ln[1]);
	// get sizes
	if (ln.size() >= 3) {
		// assign expression
		if (ln[2].type == "oper" && ln[2].val == "=") {
			p_expect_next(3);
			if (ln[3].type != "num")
				throw (string) "expected number after equals in dim";
		}
		// assign array size
		else if (ln[2].type == "bracket" && ln[2].val == "[")
			;
		// unknown
		else
			throw (string) "unexpected token in dim: " + tokstr(ln[2]);
	}
	// save
	printf("DIM    [%s]\n", ln[1].val.c_str());
}
// parse print statements
static void p_print() {
	p_expect_nonempty();
	const auto& ln = lines[lineno];
	for (int i=1; i<ln.size(); i++)
		if (ln[i].type == "ident") ;
		else if (ln[i].type == "str") ;
		else if (ln[i].type == "num") ;
		else if (ln[i].type == "ident_asstring") ;
		else if (ln[i].type == "ident_asarray") ;
		else
			throw (string) "printing unknown type: " + ln[i].val +":"+ ln[i].type;
	// save
	printf("PRINT  [%d]\n", (int)ln.size()-1);
}
// parse assignment statement
static void p_assign() {
	p_expect_nonempty();
	const auto& ln = lines[lineno];
	if (ln.size() <= 1 || ln[1].type != "oper" || ln[1].val != "=")
		throw (string) "expected = after identifier: " + ln[0].val;
	// save
	printf("ASSIGN [%s]\n", ln[0].val.c_str());
}
static void p_label() {
	p_expect_nonempty();
	p_expect_eol(1);
	const auto& lb = lines[lineno][0];
	// save
	printf("LABEL  [%s]\n", lb.val.c_str());
}
static void p_goto() {
	p_expect_nonempty();
	const auto& ln = lines[lineno];
	if (ln.size() < 2 || ln[1].type != "ident")
		throw (string) "expected identifier after goto";
	p_expect_eol(2);  // expect eol after identifier always
	// save
	printf("GOTO   [%s]\n", ln[1].val.c_str());
}
static void p_break() {
	p_expect_nonempty();
	p_expect_eol(1);
	// save
	printf("BREAK\n");
}
// parse each item in a block
static void p_block() {
	while (lineno < lines.size()) {
		// check if line is empty
		if (lines[lineno].size() == 0)
			{ lineno++;  continue; }
		const auto& cmd = lines[lineno][0];  // get command
		// parse command types
		if (cmd.type == "cmd") {
			if (cmd.val == "end") 
				p_end(),  lineno++;
			else if (cmd.val == "if" || cmd.val == "elif" || cmd.val == "while")
				p_if(),  lineno++;
			else if (cmd.val == "dim")
				p_dim(),  lineno++;
			else if (cmd.val == "print")
				p_print(),  lineno++;
			else if (cmd.val == "goto")
				p_goto(),  lineno++;
			else if (cmd.val == "break")
				p_break(),  lineno++;
			else
				throw (string) "unknown command in block: " + cmd.val;  // unknown command - break
		}
		// identifier - must be assignment
		else if (cmd.type == "ident") {
			p_assign(),  lineno++;
		}
		// label
		else if (cmd.type == "label") {
			p_label(),  lineno++;
		}
		// unknown type
		else {
			throw (string) "unexpected token in block: " + cmd.val +":"+ cmd.type;
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
		fprintf(stderr, "error [%d]: %s\n", lineno+1, err.c_str());
		return 1;
	}
}


int main() {
	pfile("test.bas");
}