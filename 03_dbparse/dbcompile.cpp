#include "dbparse.h"
#include <vector>
#include <algorithm>

using namespace std;


// static vars
static vector<string> prog;
static vector<string> labels, gotos;
static vector<pair<string, i32>> block_stack;
static int counter = 0;


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
	auto& b = block_stack.back();
	// while loop action 
	if (b.first == "while")
		prog.push_back("GOTO _while_start_" + to_string(b.second));
	// place after block jump point
	c_label("_" + b.first + "_end_" + to_string(b.second));
	block_stack.pop_back();
}
void c_if(const Expr& e) {
	block_stack.push_back({ "if", ++counter });  // next block state
	c_label("_if_start_" + to_string(counter));  // block start (needed?)
	string ex = c_expr(e);  // dump expression
	prog.push_back("IFN "+ex);
	prog.push_back("GOTO _if_end_" + to_string(counter));
}
// void c_elif(const Expr& e) {
// 	string ex = c_expr(e);
// 	if (block_stack.size() == 0 || !in_list(block_stack.back().first, { "if", "elif" }))
// 		throw (string) "unexpected ELIF outside of IF BLOCK";
// 	prog[ block_stack.back().second ] += " " + to_string(prog.size());
// 	block_stack.back() = { "elif", prog.size() };  // replace top of stack (must be if of elif) with elif
// 	prog.push_back("ELIF "+ex);
// }
// void c_else() {
// 	if (block_stack.size() == 0 || !in_list(block_stack.back().first, { "if", "elif" }))
// 		throw (string) "unexpected ELSE outside of IF BLOCK";
// 	prog[ block_stack.back().second ] += " " + to_string(prog.size());
// 	block_stack.back() = { "else", prog.size() };  // replace top of stack (must be if of elif) with else
// 	prog.push_back("ELSE");
// }
void c_while(const Expr& e) {
	block_stack.push_back({ "while", ++counter });  // next block state
	c_label("_while_start_" + to_string(counter));
	string ex = c_expr(e);
	prog.push_back("IFN "+ex);
	prog.push_back("GOTO _while_end_" + to_string(counter));
}
void c_assign(const std::string& id, const Expr& e) {
	string ex = c_expr(e);
	prog.push_back("SET " + id+" "+ex);
}
void c_label(const std::string& id) {
	// save list of unique labels for validation
	if (find(labels.begin(), labels.end(), id) == labels.end())
		labels.push_back(id);
	prog.push_back("::" + id);
}
void c_goto(const std::string& id) {
	// save list of unique gotos for validation
	if (find(gotos.begin(), gotos.end(), id) == gotos.end())
		gotos.push_back(id);
	prog.push_back("GOTO " + id);
}
// void c_break() {
// 	if (block_stack.size() == 0)
// 		throw (string) "unexpected BREAK outside of BLOCK structure";
// 	prog.push_back("BREAK");
// }
void c_eof() {
	if (block_stack.size() != 0)
		throw (string) "unterminated block: " + block_stack.back().first+"::"+to_string(block_stack.back().second);
	for (const auto& g : gotos)
		if (find(labels.begin(), labels.end(), g) == labels.end())
			throw (string) "trying to GOTO missing label: " + g;
	prog.push_back("EOF");
}
void c_print(i32 pcount) {
	printf("db_compile WARNING: unimplented compiled function print\n");
	prog.push_back("PRINT " + to_string(pcount));
}
void c_reset() {
	prog = {};
	labels = gotos = {};
	block_stack = {};
}
void c_showprog() {
	for (int i=0; i<prog.size(); i++)
		printf("% 3d) %s\n", i, prog[i].c_str());
}