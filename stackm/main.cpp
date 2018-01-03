#include "envstack.h"
#include <iostream>
#include <vector>

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


struct Expr {
	string op;
	vector<Expr> args;
};

void show(const Expr& e) {
	static int tab=0;
	printf("%s[%s]\n", string(tab*4, ' ').c_str(), e.op.c_str());
	tab++;
	for (const auto& a : e.args)
		show(a);
	tab--;
}

void test3() {
	Expr e = { 
		.op="==",
		.args={ 
			{"a"},
			{
				.op="+", 
				.args={ {"b"}, {"2"} }
			}
		}
	};
	show(e);
}


int main() {
	// stacktest();
	// test2();
	test3();
}