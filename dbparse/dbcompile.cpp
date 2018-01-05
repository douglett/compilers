#include "dbparse.h"
#include <vector>

using namespace std;


static vector<string> prog;


void c_dim(const std::string& id) {
	prog.push_back("DIM " + id + " 0");
}
void c_end() {
	prog.push_back("END");
}

void c_show_prog() {
	for (const auto& p : prog)
		printf("%s\n", p.c_str());
}