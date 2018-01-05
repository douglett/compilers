#pragma once

#include <string>
#include <cstdint>

typedef int32_t i32;

// parse
int  p_file(const std::string& fname);
// compile
void c_dim(const std::string& id);
void c_end();
void c_if();
void c_elif();
void c_while();
void c_print(i32 count);
void c_assign(const std::string& id);
void c_label(const std::string& id);
void c_goto(const std::string& id);
void c_break();
void c_eof();
void c_reset();
void c_show_prog();