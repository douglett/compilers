#include "dbparse.h"
using namespace std;

static vector<string> prog;
static i32 PC = 0;

int r_runprog(const std::vector<std::string>& program) {
	prog = program;
	PC = 0;
	
	while (PC < prog.size()) {
		PC++;
	}
	return 0;
}