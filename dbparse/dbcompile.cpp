#include "dbparse.h"
#include <vector>

using namespace std;


static vector<string> prog;
static vector<string> labels, gotos, block_stack;


void c_dim(const std::string& id) {
	prog.push_back("DIM " + id + " 1");
}
void c_end() {
	if (block_stack.size() == 0)
		throw (string) "unexpected END outside of BLOCK structure";
	block_stack.pop_back();
	prog.push_back("END");
}
void c_if() {
	block_stack.push_back("if");
	prog.push_back("IF");
}
void c_elif() {
	if (block_stack.size() == 0 || (block_stack.back() != "if" && block_stack.back() != "elif"))
		throw (string) "unexpected ELIF outside of IF BLOCK";
	block_stack.back() = "elif";  // replace top of stack (must be if of elif) with elif
	prog.push_back("ELIF");
}
void c_else() {
	if (block_stack.size() == 0 || (block_stack.back() != "if" && block_stack.back() != "elif"))
		throw (string) "unexpected ELIF outside of IF BLOCK";
	block_stack.back() = "else";  // replace top of stack (must be if of elif) with else
	prog.push_back("ELSE");
}
void c_while() {
	block_stack.push_back("while");
	prog.push_back("WHILE");
}
void c_assign(const std::string& id) {
	prog.push_back("SET " + id);
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
	prog.push_back("PRINT " + to_string(count));
}


void c_reset() {
	prog = {};
	labels = gotos = block_stack = {};
}
void c_show_prog() {
	for (const auto& p : prog)
		printf("%s\n", p.c_str());
}