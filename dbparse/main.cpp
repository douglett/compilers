#include "dbparse.h"

using namespace std;


int main() {
	if (p_file("testscripts/01_math.bas"))
		return 1;
	printf("---\n");
	c_showprog();
	printf("---\n");
	return 0;
}