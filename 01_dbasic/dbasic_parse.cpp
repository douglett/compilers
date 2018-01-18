#include "dbasic.h"
#include "parsers/pgeneral.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>

using namespace std;


static int lineno = 0;
static vector<string> proglines;
Block prog;


// helpers
// static string join(const vector<string>& vs) {
// 	string s;
// 	for (int i=0; i<vs.size(); i++)
// 		s += (i > 0 ? " " : "") + vs[i];
// 	return s;
// }
static string showtok(const Token& t) {
	return t.type + ":" + t.val;
}
static string jointok(const vector<Token>& vt) {
	string s;
	for (int i=0; i<vt.size(); i++)
		// s += (i > 0 ? " " : "") + vt[i].val;
		s += (i > 0 ? " " : "") + showtok(vt[i]);
	return s;
}
static int in_list(const string& k, const vector<string>& slist) {
	for (const auto& v : slist)
		if (k == v)  return 1;
	return 0;
}


// display functions
static const char* tabs(int size) {
	static const int TAB_SIZE = 2;
	static string s;
	s = string(size * TAB_SIZE, ' ');
	return s.c_str();
}
void showexpr(const Expr& expr, int indent=0) {
	// show current expression
	printf("%s%s\n", tabs(indent), showtok(expr.token).c_str());
	// show children
	for (const auto& e : expr.arguments)
		showexpr(e, indent+1);
}
void showstmt(const Stmt& stmt, int indent=0) {
	// show current statement
	printf("%ss: %s [%s]\n", tabs(indent), showtok(stmt.type).c_str(), jointok(stmt.modifiers).c_str());
	indent++;
	for (const auto& e : stmt.arguments)
		printf("%sse:\n", tabs(indent)),
			showexpr(e, indent+1);
}
void showblock(const Block& block, int indent=0) {
	// show block info
	printf("%sb: %s\n", tabs(indent), showtok(block.type).c_str());
	// show entry condition expression
	indent++;
	printf("%sbe:\n", tabs(indent)),
		showexpr(block.condition, indent+1);
	// show children
	for (const auto& var : block.contents) {
		if (var.type == "block")
			showblock(var.block, indent);
		else if (var.type == "stmt")
			showstmt(var.stmt, indent);
		else if (var.type == "expr")
			printf("%se: \n", tabs(indent)),
				showexpr(var.expr, indent+1);
	}
}


// parsing functions
static Token identifytok(const string& s) {
	Token tok = { .val=s, .type="???" };
	if (in_list(s, { "=", "<", ">", "<=", ">=", "+", "-", "*", "/", "and", "or" }))
		tok.type = "oper";
	else if (s == "(" || s == ")" || s == "()")
		tok.type = "brkt";
	else if (pgeneral::is_ident(s))
		tok.type = "ident";
	else if (pgeneral::is_strlit(s))
		tok.type = "str";
	else if (pgeneral::is_number(s))
		tok.type = "num";
	// printf("type %s [%s]\n", tok.type.c_str(), tok.val.c_str());
	return tok;
}
static vector<Token> tokenize(const string& line) {
	vector<Token> vt;
	auto vs = pgeneral::tokenize(line);
	for (const auto& v : vs)
		vt.push_back( identifytok(v) );
	return vt;
}
int parseexpr(Expr& expr, const vector<Token>& vt, int& pos) {
	assert(pos >= 0 && pos < vt.size());
	expr = {};
	if (vt[pos].val != "(") { expr.token = vt[pos++];  return 0; }
	// list
	expr.token = identifytok("()");
	pos++;
	while (pos < vt.size()) {
		if (vt[pos].val == ")") { pos++;  break; }
		else {
			Expr e;
			parseexpr(e, vt, pos);
			expr.arguments.push_back(e);
		}
	}
	return 0;
}
int parsestmt(Stmt& stmt, const vector<Token>& vt, int& pos) {
	assert(pos >= 0 && pos < vt.size());
	stmt = { .type=vt[pos++] };
	// get statement modifiers - first argument must be identifier
	if (in_list(stmt.type.val, { "let", "call" })) {
		assert(vt.size() >= 2);
		stmt.modifiers.push_back( vt[pos++] );
	}
	// get expression arguments
	Expr e;
	while (pos < vt.size())
		parseexpr(e, vt, pos),
			stmt.arguments.push_back(e);
	return 0;
}


//-- parsing blocks
static Block pcond();
static Block pprog();
// inner block parsing
static int parseblock(Block& block) {
	while (lineno < proglines.size()) {
		// tokenize line
		auto vt = tokenize( proglines[lineno] );
		if (vt.size() == 0)  
			{ lineno++;  continue; }
		else if (vt[0].val == "end")
			{ lineno++;  break; }
		else if (in_list(vt[0].val, { "if", "while" }))
			block.contents.push_back({ .type = "block", .block = pcond() });
		else if (pgeneral::is_ident(vt[0].val)) {
			lineno++;
			// parse Statement into AstVar container
			Stmt stmt;
			int pos = 0;
			parsestmt(stmt, vt, pos);
			block.contents.push_back({ .type="stmt", .block={}, .stmt=stmt, .expr={} });
		}
		else {
			lineno++;
			// parse Expression into AstVar container
			Expr expr;
			int pos = 0;
			parseexpr(expr, vt, pos);
			block.contents.push_back({ .type="expr", .block={}, .stmt={}, .expr=expr });
		}
		// else 
		// 	block.contents.push_back({ .type = "stmt", .expr = pstmtall() });
	}
	return 0;
}
// conditionals (if, while)
static Block pcond() {
	auto vt = tokenize( proglines[lineno++] );
	Block b = { .type = vt[0] };
	int pos = 1;
	parseexpr(b.condition, vt, pos);  // parse block entry condition
	parseblock(b);  // parse inner block
	return b;
}
// entire program
static Block pprog() {
	Block prog = Block{ .type=identifytok("program") };
	parseblock(prog);
	return prog;
}


int parsefile(const std::string& fname) {
	fstream fs(fname, fstream::in);
	string s;
	if (!fs.is_open())
		{ fprintf(stderr, "error: could not open file: %s\n", fname.c_str());  return 1; }
	// reset
	lineno = 0;
	proglines = {};
	// getlines
	while (getline(fs, s))
		proglines.push_back(s);
	prog = pprog();
	showblock(prog);
	return 0;
}


int main() {
	if (parsefile("test.bas"))  return 1;
	if (runprog(prog))  return 1;
	return 0;
}
