#include "dbparse.h"

using namespace std;


int main() {
	if (p_file("test.bas"))
		return 1;
	printf("---\n");
	c_showprog();
	printf("---\n");
	if (r_runprog( c_program() ))
		return 1;
	return 0;
}