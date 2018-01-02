#include "envstack.h"
#include <iostream>

using namespace std;


void stacktest() {
	try {
		lframe();
			ldef("PC", 1);
			lframe();
				ldef("foo", 1);
				llet("foo", 20);
				lleta("foo", 0, 21);
				printf("> [%d]\n", loper("+", 10, lget("foo")) );
				printf("> [%d]\n", lget("PC") );
			lunframe();
		lunframe();
	}
	catch (const string& e) {
		fprintf(stderr, "error: %s\n", e.c_str());
	}
}


void test2() {
	try {
		lframe();

		ldef("foo", 10);
		ldef("i", 1);
		llet("i", 9);
		while (loper(">=", lget("i"), 0)) {
			lleta("foo", lget("i"), 'z' - lget("i"));
			llet("i", loper("-", lget("i"), 1));
		}
		printf("> %s\n", lgetstr("foo").c_str());

		lunframe();
	}
	catch (const string& e) {
		fprintf(stderr, "error: %s\n", e.c_str());
	}
}


int main() {
	// stacktest();
	test2();
}