#include "dbparse.h"

using namespace std;


int main() {
	if (p_file("test.bas"))
		return 1;
	printf("---\n");
	c_show_prog();
	return 0;
}