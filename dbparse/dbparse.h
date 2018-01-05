#pragma once

#include <string>
#include <cstdint>

typedef int32_t i32;

// parse
int  p_file(const std::string& fname);
// compile
void c_dim(const std::string& id);
void c_end();
void c_show_prog();