#include "dbparse.h"

using namespace std;

int in_list(const std::string& k, const std::vector<std::string>& vs) {
	for (const auto& s : vs)
		if (s == k)  return 1;
	return 0;
}

std::vector<std::string> split(const std::string& s) {
	vector<string> vs={""};
	for (char c : s)
		if   (isspace(c)) { if (vs.back().length())  vs.push_back(""); }  // new entry
		else { vs.back() += c; }
	if (vs.size() > 1 && vs.back().length() == 0)  vs.pop_back();  // return at least one entry
	return vs;
}