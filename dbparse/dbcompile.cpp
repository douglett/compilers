#include "dbparse.h"
#include <vector>
#include <algorithm>

using namespace std;


// static vars
// static const int dtrace = 0;
static vector<string> prog;
static vector<string> labels, gotos, block_stack;


// static void c_expr(const Expr& e) {
// 	for (const Expr& ee : e.c)
// 		c_expr(ee);
// 	if (e.tok.type == "ident" || e.tok.type == "num")
// 		prog.push_back("SET _top " + e.tok.val);
// 	else if (e.tok.type == "bracket")
// 		; // ignore
// 	else if (e.tok.type == "oper")
// 		prog.push_back("OP"+e.tok.val+" _1 _2");
// 	// prog.push_back("   EXPR " + e.tok.val);
// }


static string c_expr(const Expr& e) {
	if (e.c.size() == 0)
		return e.tok.val;
	if (e.c.size() == 2) {
		string l = c_expr(e.c[0]);
		string r = c_expr(e.c[1]);
		prog.push_back("OP"+e.tok.val + " "+l+" "+r);
		return "_top";
	}
	if (e.c.size() == 1 && e.tok.type == "bracket")
		return c_expr(e.c[0]);
	throw (string) "error in EXPRESSION";
}


void c_dim(const std::string& id, i32 size) {
	prog.push_back("DIM " + id + " " + to_string(size));
}
void c_end() {
	if (block_stack.size() == 0)
		throw (string) "unexpected END outside of BLOCK structure";
	block_stack.pop_back();
	prog.push_back("END");
}
void c_if(const Expr& e) {
	string ex = c_expr(e);
	block_stack.push_back("if");
	prog.push_back("IF "+ex);
}
void c_elif(const Expr& e) {
	string ex = c_expr(e);
	if (block_stack.size() == 0 || (block_stack.back() != "if" && block_stack.back() != "elif"))
		throw (string) "unexpected ELIF outside of IF BLOCK";
	block_stack.back() = "elif";  // replace top of stack (must be if of elif) with elif
	prog.push_back("ELIF "+ex);
}
void c_else() {
	if (block_stack.size() == 0 || (block_stack.back() != "if" && block_stack.back() != "elif"))
		throw (string) "unexpected ELIF outside of IF BLOCK";
	block_stack.back() = "else";  // replace top of stack (must be if of elif) with else
	prog.push_back("ELSE");
}
void c_while(const Expr& e) {
	string ex = c_expr(e);
	block_stack.push_back("while");
	prog.push_back("WHILE "+ex);
}
void c_assign(const std::string& id, const Expr& e) {
	string ex = c_expr(e);
	prog.push_back("SET " + id+" "+ex);
}
void c_label(const std::string& id) {
	// save list of unique labels for validation
	if (find(labels.begin(), labels.end(), id) == labels.end())
		labels.push_back(id);
	prog.push_back("LABEL " + id);
}
void c_goto(const std::string& id) {
	// save list of unique gotos for validation
	if (find(gotos.begin(), gotos.end(), id) == gotos.end())
		gotos.push_back(id);
	prog.push_back("GOTO " + id);
}
void c_break() {
	if (block_stack.size() == 0)
		throw (string) "unexpected BREAK outside of BLOCK structure";
	prog.push_back("BREAK");
}
void c_eof() {
	if (block_stack.size() != 0)
		throw (string) "unterminated block: " + block_stack.back();
	for (const auto& g : gotos)
		if (find(labels.begin(), labels.end(), g) == labels.end())
			throw (string) "trying to GOTO missing label: " + g;
	prog.push_back("EOF");
}


void c_print(i32 count) {
	printf("db_compile WARNING: unimplented compiled function print\n");
	prog.push_back("PRINT " + to_string(count));
}


void c_reset() {
	prog = {};
	labels = gotos = block_stack = {};
}
void c_showprog() {
	for (const auto& p : prog)
		printf("%s\n", p.c_str());
}
const std::vector<std::string>& c_program() {
	return prog;
}
