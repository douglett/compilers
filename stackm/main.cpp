#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>

using namespace std;


typedef int32_t i32;
typedef map<string, vector<i32>> LFrame;
static vector<LFrame> frames;
// static vector<LComm>  commands;
// static i32 lPC = 0;


void lframe() {
	frames.push_back({});
}
void lunframe() {
	if (!frames.size()) 
		throw "attempt to pop frame stack while list is empty";
	frames.pop_back();
}
// LFrame& lgetframe() {
// 	if (!frames.size())
// 		throw "attempt to get frame from empty stack";
// 	return frames.back();
// }
void ldef(const string& id, i32 size) {
	if (!frames.size())
		throw (string) "attempt to def on empty stack";
	if (frames.back().count(id))
		throw (string) "attempt to def existing variable name: " + id;
	if (size < 1 || size > 256)
		throw (string) "def size out of range: " + to_string(size);
	frames.back()[id] = vector<i32>(size, 0);
}
// helper - find var in stack
static vector<i32>& lgetvar(const string& id) {
	if (!frames.size())
		throw (string) "attempt to access on empty stack";
	for (int i = frames.size()-1; i >= 0; i--)
		if (frames[i].count(id))
			return frames[i][id];
	throw (string) "attempt to access value on undefined var: " + id;
}
i32 lgeta(const string& id, i32 pos) {
	auto& v = lgetvar(id);  // get from stack. does error checking
	if (pos < 0 || pos >= v.size())
		throw (string) "index out of range on var " + id + " : " + to_string(pos);
	return v[pos];
}
void lleta(const string& id, i32 pos, i32 val) {
	auto& v = lgetvar(id);  // get from stack. does error checking
	if (pos < 0 || pos >= v.size())
		throw (string) "index out of range on var " + id + " : " + to_string(pos);
	v[pos] = val;
}
i32 lget(const string& id) {
	return lgeta(id, 0);
}
void llet(const string& id, i32 val) {
	lleta(id, 0, val);
}
i32 loper(const string& op, i32 v1, i32 v2) {
	if      (op == "+" )  return v1 +  v2;
	else if (op == "-" )  return v1 -  v2;
	else if (op == "=" || op == "==")  return v1 == v2;
	else if (op == "!=")  return v1 != v2;
	else    throw "unknown operator: " + op;
}


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