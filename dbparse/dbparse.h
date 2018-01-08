#pragma once

#include <string>
#include <vector>
#include <cstdint>

typedef int32_t i32;
struct Token {
	std::string val;
	std::string type; };
struct Expr {
	Token tok;
	std::vector<Expr> c; };

// parse
int  p_file(const std::string& fname);
// compile
void c_dim(const std::string& id, i32 size);
void c_end();
void c_if(const Expr& e);
void c_elif(const Expr& e);
void c_else();
void c_while(const Expr& e);
void c_assign(const std::string& id, const Expr& e);
void c_label(const std::string& id);
void c_goto(const std::string& id);
void c_break();
void c_eof();
void c_print(i32 count);  // special commands
void c_reset();  // meta commands
void c_showprog();
const std::vector<std::string>&
     c_program();
// run
int  r_runprog(const std::vector<std::string>& program);