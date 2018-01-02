#include "dbasic.h"
#include "pgeneral.h"
#include <iostream>
#include <map>
#include <cassert>

using namespace std;


static map<string, Block> envfn;
static map<string, i32> env;
static int rtrace = 0;


// helpers
static int in_list(const string& k, const vector<string>& slist) {
	for (const auto& v : slist)
		if (k == v)  return 1;
	return 0;
}


// predef
static i32 runblock(const Block& block);
static i32 runstmt(const Stmt& stmt);
// run
static i32 runexpr(const Expr& expr) {
	if (expr.token.type == "num")
		return pgeneral::strtonum(expr.token.val);
	else if (expr.token.type == "ident")
		return ( env.count(expr.token.val) ? env[expr.token.val] : 0 );
	else if (expr.token.type == "brkt") {
		assert(expr.arguments.size() >= 2 && expr.arguments[0].token.type == "oper");
		auto op = expr.arguments[0].token.val;
		// math operations
		if (in_list(op, { "+", "-", "*", "/" })) {
			i32 res = runexpr(expr.arguments[1]);
			for (int i=2; i<expr.arguments.size(); i++)
				if      (op == "+")  res += runexpr(expr.arguments[i]);
				else if (op == "-")  res -= runexpr(expr.arguments[i]);
				else if (op == "*")  res *= runexpr(expr.arguments[i]);
				else if (op == "/")  res /= runexpr(expr.arguments[i]);
			return res;
		}
		// equality operations
		else {
			i32 res = 1;
			i32 cmp = runexpr(expr.arguments[1]);
			for (int i=2; i<expr.arguments.size(); i++) {
				if      (op == "=" )  res = (runexpr(expr.arguments[i]) == cmp);
				else if (op == "<" )  res = (runexpr(expr.arguments[i]) <  cmp);
				else if (op == ">" )  res = (runexpr(expr.arguments[i]) >  cmp);
				else if (op == "<=")  res = (runexpr(expr.arguments[i]) <= cmp);
				else if (op == ">=")  res = (runexpr(expr.arguments[i]) >= cmp);
				if (res == 0)  return 0;
			}
			return 1;
		}
	}
	fprintf(stderr, "error: unknown type in expr: %s\n", expr.token.type.c_str());
	exit(1);
}
static i32 runstmt(const Stmt& stmt) {
	if (rtrace)  printf("stmt [%s]\n", stmt.type.val.c_str());
	// let statement - assign
	if (stmt.type.val == "let") {
		assert( stmt.modifiers.size() == 1 && stmt.arguments.size() == 1 );
		env[stmt.modifiers[0].val] = runexpr(stmt.arguments[0]);
		return 0;
	}
	// print statement - eval/run each item
	else if (stmt.type.val == "print") {
		printf("> ");
		for (const auto& m : stmt.modifiers)
			printf("%s ", m.val.c_str());
		for (const auto& a : stmt.arguments)
			if (a.token.type == "str")  printf("%s ", a.token.val.substr(1, a.token.val.size()-2).c_str());
			else  printf("%d ", runexpr(a));
		printf("\n");
		return 0;
	}
	// call statement - simple sub call
	else if (stmt.type.val == "call") {
		assert( stmt.modifiers.size() > 0 );
		auto id = stmt.modifiers[0].val;
		if (rtrace)  printf("call %s\n", id.c_str());
		if (envfn.count(id) == 0)
			fprintf(stderr, "error: function not defined: %s\n", id.c_str()),  exit(1);
		runblock( envfn[id] );
		return 0;
	}
	fprintf(stderr, "error: unknown statement: %s\n", stmt.type.val.c_str());
	exit(1);
}
static i32 runblock(const Block& block) {
	if (rtrace)  printf("blck [%s] %d\n", block.type.val.c_str(), runexpr(block.condition));
	// check block entry condition
	block_top:
	if (block.type.val != "function" && runexpr(block.condition) == 0)
		return 0;
	// run each
	for (const auto& var : block.contents)
		// run statement or expression
		if (var.type == "expr") 
			{ i32 res = runexpr(var.expr);  if (rtrace) printf("expr %d\n", res); }
		else if (var.type == "stmt")
			runstmt(var.stmt);
		// define function block
		else if (var.type == "block" && var.block.type.val == "function")
			{ auto id = var.block.condition.token.val;  envfn[id] = var.block; }
		// run block
		else if (var.type == "block")
			runblock(var.block);
	// check if we need to loop
	if (block.type.val == "while")
		goto block_top;
	return 0;
}
int runprog(const Block& block) {
	return runblock(block);
}