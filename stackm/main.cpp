#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>

using namespace std;


typedef int32_t i32;
typedef map<string, vector<i32>> LFrame;
static vector<LFrame> frames;
static i32 lPC = 0;


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
		throw "attempt to def on empty stack";
	if (frames.back().count(id))
		throw "attempt to def existing variable name: " + id;
	if (size < 1 || size > 256)
		throw "def size out of range: " + to_string(size);
	frames.back()[id] = vector<i32>(size, 0);
}
i32 lgeta(const string& id, i32 pos, i32 val) {
	if (!frames.size())
		throw "attempt to access on empty stack";
	if (frames.back().count(id) == 0)
		throw "attempt to access value on undefined var: " + id;
	auto& v = frames.back()[id];
	if (pos < 0 || pos >= v.size())
		throw "index out of range on var " + id + " : " + to_string(pos);
	return v[pos];
}
void lleta(const string& id, i32 pos, i32 val) {
	lgeta(id, pos, val);  // error checking
	auto& v = frames.back()[id];
	v[pos] = val;
}
i32 lget(const string& id, i32 val) {
	return lgeta(id, 0, val);
}
void llet(const string& id, i32 val) {
	lleta(id, 0, val);
}
void llabel(const string& id) {
	throw "how do I set labels?";
}


int main() {
	try {
		lframe();
		ldef("foo", 1);
		llet("foo", 20);
		lleta("foo", 0, 20);
		// llabel("foolabel");
		lunframe();
	}
	catch (const string& e) {
		throw e.c_str();
	}
	catch (const char* e) {
		fprintf(stderr, "error: %s\n", e);
	}
}