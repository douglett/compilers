#include "dbparse.h"
#include <vector>

using namespace std;


static vector<string> prog;
static int block_count = 0;
static vector<string> labels, gotos;


void c_dim(const std::string& id) {
	prog.push_back("DIM " + id + " 1");
}
void c_end() {
	block_count--;
	prog.push_back("END");
}
void c_if() {
	block_count++;
	prog.push_back("IF");
}
void c_elif() {
	prog.push_back("ELIF");
}
void c_else() {
	prog.push_back("ELSE");
}
void c_while() {
	block_count++;
	prog.push_back("WHILE");
}
void c_print(i32 count) {
	prog.push_back("PRINT " + to_string(count));
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
	prog.push_back("BREAK");
}
void c_eof() {
	if (block_count != 0)
		throw (string) "mismatch between BLOCK statements and END";
	for (const auto& g : gotos)
		if (find(labels.begin(), labels.end(), g) == labels.end())
			throw (string) "trying to GOTO missing label: " + g;
	prog.push_back("EOF");
}


void c_reset() {
	prog = {};
	block_count = 0;
	labels = gotos = {};
}
void c_show_prog() {
	for (const auto& p : prog)
		printf("%s\n", p.c_str());
}