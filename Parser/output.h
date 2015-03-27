#pragma once
#ifndef OUTPUT_H
#define OUTPUT_H

/// This intends to be a C++ header

#include <string>
#include "types.h"
using std::string;

void print_element_list(string *out, element *elt, int format, int exts);

#endif  // OUTPUT_H
