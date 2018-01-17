#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

struct Node {
	string v;
	vector<Node> c;
};

// helpers
static Node split(const string& str) {
	stringstream ss(str);
	string s;
	Node n;
	while (ss >> s)
		n.c.push_back({ s, {} });
	return n;
}
static vector<Node> subvec(const vector<Node>& v, int start, int len) {
	if (start < 0)  start = 0;
	if (start >= v.size())  return {};
	if (len == -1)  len = v.size() - start;
	return vector<Node>( v.begin()+start, v.begin()+start+len );
}

// parsing
static Node brackets(Node n) {
	int bstart, bend;
	start:
	bstart = bend = -1;
	for (int i=0; i<n.c.size(); i++) {
		if (n.c[i].v == "(")  bstart = i;
		if (n.c[i].v == ")")  bend = i;
		if (bstart > -1 && bend > -1) {
			Node n2 = { "()", subvec(n.c, bstart+1, bend-bstart-1) };
			n.c.erase(n.c.begin()+bstart, n.c.begin() + bend+1);
			n.c.insert(n.c.begin()+bstart, n2);
			goto start;
		}
	}
	return n;
}
static Node spliton(Node n, const vector<string>& op) {
	for (int i=n.c.size()-1; i>=0; i--)  // walk backwards (left associate)
		for (const auto& o : op)
			if (n.c[i].v == o) {
				Node n2 = n.c[i];
				// lhs
				if (i == 1)  n2.c.push_back(n.c[0]);
				else if (i > 0)  n2.c.push_back({ "", subvec(n.c, 0, i) });
				// rhs
				if (i == n.c.size()-2)  n2.c.push_back(n.c.back());
				else if (i+1 < n.c.size())  n2.c.push_back({ "", subvec(n.c, i+1, -1) });
				// override or child node?
				if (n.v == "")  n = n2;
				else  n.c = { n2 };
				goto postloop;
			}
	postloop:
	// do sub-lists
	for (int i=n.c.size()-1; i>=0; i--)
		n.c[i] = spliton(n.c[i], op);
	return n;
}

void show(const Node& n) {
	static int indent = 0;
	printf("%s[%s]\n", string(indent*4, ' ').c_str(), n.v.c_str());
	for (const auto& nn : n.c) {
		indent++;
		show(nn);
		indent--;
	}
}

Node parse(const string& str) {
	Node n = split(str);
	n = brackets(n);
	n = spliton(n, { "==", "!=", "<=", ">=", "<", ">" });
	n = spliton(n, { "+", "-" });
	n = spliton(n, { "*", "/" });
	return n;
}

int main() {
	vector<string> fuck = {
		"1 + 2",
		"1 + 2 + 3 - 4 - 5",
		"1 + 2 * 3 + 4 * 4",
		"1 + 2 + ( 3 * 4 )",
		"( 1 ) + ( 2 )",
		"2 + ( 2 - 3 ) + 4",
		"( 2 + 3 ) <= 2 + 4"
	};
	for (const auto& f : fuck) {
		Node n = parse(f);
		show(n);
		printf("--\n");
	}
}