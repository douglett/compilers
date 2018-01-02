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
i32 lgeta(const string& id, i32 pos) {
	if (!frames.size())
		throw (string) "attempt to access on empty stack";
	if (frames.back().count(id) == 0)
		throw (string) "attempt to access value on undefined var: " + id;
	auto& v = frames.back()[id];
	if (pos < 0 || pos >= v.size())
		throw (string) "index out of range on var " + id + " : " + to_string(pos);
	return v[pos];
}
void lleta(const string& id, i32 pos, i32 val) {
	lgeta(id, pos);  // error checking
	auto& v = frames.back()[id];
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
		ldef("foo", 1);
		llet("foo", 20);
		lleta("foo", 0, 21);
		printf("> [%d]\n", loper("+", 10, lget("foo")) );
		lunframe();
	}
	catch (const string& e) {
		fprintf(stderr, "error: %s\n", e.c_str());
	}
}