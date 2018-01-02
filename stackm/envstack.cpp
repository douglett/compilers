#include "envstack.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;


//-- private vars
typedef map<string, vector<i32>> LFrame;
static vector<LFrame> frames;
// static vector<LComm>  commands;
// static i32 lPC = 0;


//-- private helpers
// find var in stack
static vector<i32>& lgetvar(const string& id) {
	if (!frames.size())
		throw (string) "attempt to access on empty stack";
	for (int i = frames.size()-1; i >= 0; i--)
		if (frames[i].count(id))
			return frames[i][id];
	throw (string) "attempt to access value on undefined var: " + id;
}


//-- public methods
// push frame
void lframe() {
	frames.push_back({});
}
// pop frame
void lunframe() {
	if (!frames.size()) 
		throw "attempt to pop frame stack while list is empty";
	frames.pop_back();
}
// define var in frame
void ldef(const string& id, i32 size) {
	if (!frames.size())
		throw (string) "attempt to def on empty stack";
	if (frames.back().count(id))
		throw (string) "attempt to def existing variable name: " + id;
	if (size < 1 || size > 256)
		throw (string) "def size out of range: " + to_string(size);
	frames.back()[id] = vector<i32>(size, 0);
}
// get var at array index
i32 lgeta(const string& id, i32 pos) {
	auto& v = lgetvar(id);  // get from stack. does error checking
	if (pos < 0 || pos >= v.size())
		throw (string) "index out of range on var " + id + " : " + to_string(pos);
	return v[pos];
}
// set var at array index
void lleta(const string& id, i32 pos, i32 val) {
	auto& v = lgetvar(id);  // get from stack. does error checking
	if (pos < 0 || pos >= v.size())
		throw (string) "index out of range on var " + id + " : " + to_string(pos);
	v[pos] = val;
}
// get and set at index 0 (simple var)
i32 lget(const string& id) {
	return lgeta(id, 0);
}
void llet(const string& id, i32 val) {
	lleta(id, 0, val);
}
// run operation
i32 loper(const string& op, i32 v1, i32 v2) {
	if      (op == "+" )  return v1 +  v2;
	else if (op == "-" )  return v1 -  v2;
	else if (op == "=" || op == "==")  return v1 == v2;
	else if (op == "!=")  return v1 != v2;
	else    throw "unknown operator: " + op;
}