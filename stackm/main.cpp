#include "envstack.h"
#include <iostream>

using namespace std;


int main() {
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