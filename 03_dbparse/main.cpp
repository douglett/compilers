#include "dbparse.h"

using namespace std;


int main() {
	int err = 0;
	// err = p_file("testscripts/01_firsttest.bas");
	err = p_file("testscripts/02_math.bas");
	if (err)  return 1;
	printf("---\n");
	c_showprog();
	printf("---\n");
	return 0;
}