#pragma once

#include <string>
#include <vector>
#include <cstdint>

typedef int32_t i32;

struct AstVar;  // predef
struct Token {
	std::string val;
	std::string type; };
struct Expr {
	Token token;
	std::vector<Expr> arguments; };
struct Stmt {
	Token type;
	std::vector<Token> modifiers;
	std::vector<Expr>  arguments; };
struct Block {
	Token type;
	Expr  condition;
	std::vector<AstVar> contents; };
struct AstVar {  // variadic
	std::string type;
	Block  block;
	Stmt   stmt;
	Expr   expr; };

// methods
int parsefile(const std::string& fname);
int runprog(const Block& block);