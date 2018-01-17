#include "dbparse.h"
#include "parsers/pgeneral.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;


// static stuff
static const int dtrace = 1;
static vector<vector<Token>> lines;
static int lineno = 0;


// helpers
static Token identifytok(const string& s) {
	Token tok = { .val=s, .type="???" };
	if (s.size() == 0)
		;  // shouldn't happen
	else if (in_list(s, { "end", "if", "elif", "else", "while", "goto", "break", "dim", "print" }))
		tok.type = "cmd";
	else if (in_list(s, { "=", "==", "!=", "<", ">", "<=", ">=", "+", "-", "*", "/", "&&", "||" }))
		tok.type = "oper";
	else if (in_list(s, { "_top" }))
		tok.type = "reserved";
	else if (in_list(s, { "(", ")", "()", "[", "]" }))
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
static void show_expr(const Expr& e) {
	static int indent = 1;
	printf("%s[%s]\n", string(indent*3,' ').c_str(), tokstr(e.tok).c_str());
	indent++;
	for (const auto& ee : e.c)
		show_expr(ee);
	indent--;
}


// basic checks
static void p_expect_eol(int pos) {
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


// parse expression brackets
static Expr p_e_bracket(Expr e) {
	int bstart, bend;
	start:
	// find innermost bracket start and end
	bstart = bend = -1;
	for (int i=0; i<e.c.size(); i++) {
		if (e.c[i].tok.val == "(")  bstart = i;
		if (e.c[i].tok.val == ")")  bend = i;
		// if a start and end were given, squash, then loop
		if (bstart >= 0 && bend >= 0) {
			vector<Expr> subvec( e.c.begin() + bstart+1, e.c.begin() + bend );
			e.c.erase( e.c.begin() + bstart, e.c.begin() + bend + 1 );
			e.c.insert( e.c.begin() + bstart, { .tok=identifytok("()"), .c=subvec } );
			goto start;
		}
	}
	// if only one was found, must be bracket mismatch
	if (bstart >= 0 || bend >= 0)
		throw (string) "bracket mismatch in EXPRESSION: " + to_string(bstart) + " " + to_string(bend);
	// special case - first item is a bracket
	if (e.tok.val == "" && e.c.size() == 1)  return e.c[0];
	return e;
}
// parse expression operator
static Expr p_e_oper(Expr e, const vector<string>& oplist) {
	// single empty item - just go up a step
	if (e.tok.val == "" && e.c.size() == 1)
		{ auto temp = e.c[0];  e = temp; }  // NOTE: e = e.c[0] without temp var causes strange results!
	// check all operators
	for (int i=e.c.size()-1; i>=0; i--) {
		// operator found - sqashify sublist
		if (e.c[i].tok.type == "oper" && in_list(e.c[i].tok.val, oplist)) {
			Expr e2 = e.c[i];
			e2.c.push_back({ .tok=Token(), .c=vector<Expr>(e.c.begin(), e.c.begin()+i) });
			e2.c.push_back({ .tok=Token(), .c=vector<Expr>(e.c.begin()+i+1, e.c.end()) });
			if (e.tok.val == "")  e = e2;
			else  e.c = { e2 };
			break;
		}
	}
	// check all sublists for operators
	for (int i=e.c.size()-1; i>=0; i--)
		e.c[i] = p_e_oper(e.c[i], oplist);
	return e;
}
// finally, validate
static void p_e_validate(const Expr& e) {
	if (e.tok.type == "bracket") {
		if (e.tok.val != "()" || e.c.size() != 1)
			throw (string) "expected bracket () with single child in EXPRESSION"; }
	else if (e.tok.type == "ident" || e.tok.type == "num") {
		if (e.c.size() != 0)
			throw (string) "number or identifier with unexpected children in EXPRESSION"; }
	else if (e.tok.type == "oper") {
		if (e.c.size() != 2)
			throw (string) "expected 2 arguments after operator in EXPRESSION"; }
	else {
		throw (string) "unexpected token in EXPRESSION: " + tokstr(e.tok); }
	// validate children
	for (const auto& e2 : e.c)
		p_e_validate(e2);
}
// parse full expression
static Expr p_expression(int& pos) {
	p_expect_next(pos);
	const auto& ln = lines[lineno];
	Expr e;
	for ( ; pos < ln.size(); pos++)
		if (in_list(ln[pos].type, { "num", "ident", "oper", "bracket" }))
			e.c.push_back({ .tok=ln[pos] });
	// parse items according to operator precedence
	const int edebug = 0;
	if (edebug)  printf("start\n"),  show_expr(e),  printf("bracket\n");
	e = p_e_bracket(e);
	if (edebug)  show_expr(e),  printf("or\n");
	e = p_e_oper(e, { "||" });
	if (edebug)  show_expr(e),  printf("and\n");
	e = p_e_oper(e, { "&&" });
	if (edebug)  show_expr(e),  printf("eq\n");
	e = p_e_oper(e, { "==", "!=", "<", ">", "<=", ">=" });
	if (edebug)  show_expr(e),  printf("add\n");
	e = p_e_oper(e, { "+", "-" });
	if (edebug)  show_expr(e),  printf("mul\n");
	e = p_e_oper(e, { "*", "/" });
	if (edebug)  show_expr(e);
	// finally, validate resulting expression tree
	p_e_validate(e);
	// return results
	if (dtrace)  show_expr(e);
	return e;
}


// parse end
static void p_end() {
	p_expect_nonempty();
	p_expect_eol(1);
	// save
	if (dtrace)  printf("END\n");
	c_end();
}
// parse if or elif statement
static void p_if() {
	p_expect_nonempty();
	const auto& ln = lines[lineno];
	const string& type = ln[0].val;
	// make sure expression inside braces (needed?)
	p_expect_next(1);
	if (ln[1].type != "bracket" || ln[1].val != "(")
		throw (string) "expected bracketed expression after " + type;
	p_expect_next(2);
	if (ln.back().type != "bracket" || ln.back().val != ")")
		throw (string) "expected close bracket after " + type;
	// get expression
	int pos = 1;
	Expr e = p_expression(pos);
	p_expect_eol(pos);
	// save
	if (type == "if") {
		if (dtrace)  printf("IF     [expr]\n");
		c_if(e);
	}
	// else if (type == "elif") {
	// 	if (dtrace)  printf("ELIF   [expr]\n");
	// 	c_elif(e);
	// }
	else if (type == "while") {
		if (dtrace)  printf("WHILE  [expr]\n");
		c_while(e);
	}
	else
		throw (string) "unknown type in if: " + type;
}
// parse else statement - no conditional
// static void p_else() {
// 	p_expect_nonempty();
// 	p_expect_eol(1);
// 	// save
// 	if (dtrace)  printf("ELSE\n");
// 	c_else();
// }
// parse dim statements
static void p_dim() {
	p_expect_nonempty();
	const auto& ln = lines[lineno];
	if (ln.size() <= 1)
		throw (string) "expected ident after dim, got eol";
	if (ln[1].type != "ident")
		throw (string) "expected ident after dim, got: " + tokstr(ln[1]);
	// get sizes
	int dimsize = 1;
	if (ln.size() >= 3) {
		// parse as sized array
		if (ln[2].type == "bracket" && ln[2].val == "[") {
			p_expect_next(3);
			int pos = 3;
			if (ln[pos].type == "num")
				dimsize = pgeneral::strtonum( ln[pos++].val );
			if (ln[pos].type != "bracket" || ln[pos].val != "]")
				throw (string) "expected closing bracket after size in dim";
		}
		// assign expression to var
		else if (ln[2].type == "oper" && ln[2].val == "=") {
			p_expect_next(3);
		}
		// unknown
		else
			throw (string) "unexpected token in dim: " + tokstr(ln[2]);
	}
	// save
	if (dtrace)  printf("DIM    [%s]\n", ln[1].val.c_str());
	c_dim(ln[1].val, dimsize);
}
// parse assignment statement
static void p_assign() {
	p_expect_nonempty();
	const auto& ln = lines[lineno];
	if (ln.size() <= 1 || ln[1].type != "oper" || ln[1].val != "=")
		throw (string) "expected = after identifier: " + ln[0].val;
	p_expect_next(2);
	int pos = 2;
	auto e = p_expression(pos);
	// save
	if (dtrace)  printf("ASSIGN [%s]\n", ln[0].val.c_str());
	// c_assign(ln[0].val, { .tok=identifytok("0") });
	c_assign(ln[0].val, e);
}
// parse label statement line
static void p_label() {
	p_expect_nonempty();
	p_expect_eol(1);
	const auto& lb = lines[lineno][0];
	// save
	if (dtrace)  printf("LABEL  [%s]\n", lb.val.c_str());
	c_label(lb.val.substr(0, lb.val.size()-1));
}
// parse goto statement
static void p_goto() {
	p_expect_nonempty();
	const auto& ln = lines[lineno];
	if (ln.size() < 2 || ln[1].type != "ident")
		throw (string) "expected identifier after goto";
	p_expect_eol(2);  // expect eol after identifier always
	// save
	if (dtrace)  printf("GOTO   [%s]\n", ln[1].val.c_str());
	c_goto(ln[1].val);
}
// parse break statement
// static void p_break() {
// 	p_expect_nonempty();
// 	p_expect_eol(1);
// 	// save
// 	if (dtrace)  printf("BREAK\n");
// 	c_break();
// }
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
			throw (string) "printing unknown type: " + tokstr(ln[i]);
	// save
	if (dtrace)  printf("PRINT  [%d]\n", (int)ln.size()-1);
	c_print(ln.size()-1);
}


// parse each item in the program block
static void p_block() {
	if (dtrace)  printf("PROG_START\n");
	while (lineno < lines.size()) {
		// check if line is empty
		if (lines[lineno].size() == 0)
			{ lineno++;  continue; }
		const auto& cmd = lines[lineno][0];  // get command
		// parse command types
		if (cmd.type == "cmd") {
			if (cmd.val == "end") 
				p_end(),  lineno++;
			else if (cmd.val == "if" || cmd.val == "while")
				p_if(),  lineno++;
			// else if (cmd.val == "else")
				// p_else(),  lineno++;
			else if (cmd.val == "dim")
				p_dim(),  lineno++;
			else if (cmd.val == "goto")
				p_goto(),  lineno++;
			// else if (cmd.val == "break")
			// 	p_break(),  lineno++;
			else if (cmd.val == "print")
				p_print(),  lineno++;
			else
				throw (string) "unknown command in block: " + tokstr(cmd);  // unknown command - break
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
			throw (string) "unexpected token in block: " + tokstr(cmd);
		}
	}
	// check final program structure
	c_eof();
	if (dtrace)  printf("PROG_EOF\n");
}


int p_file(const string& fname) {
	printf("parsing file: %s\n", fname.c_str());
	// reset
	lines = {}, lineno = 0;
	pgeneral::punclist = "()[]";
	// open and save tokens
	fstream fs(fname, fstream::in);
	if (!fs.is_open()) {
		fprintf(stderr, "error: file not found: %s\n", fname.c_str());
		return 1;
	}
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
