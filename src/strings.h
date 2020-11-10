//
// Created by mats on 2020-11-08.
//

#ifndef MATSLISP_SRC_STRINGS_H_
#define MATSLISP_SRC_STRINGS_H_

#include "object.h"
#include <stdlib.h>

int is_digit10(int c);
int is_letter(int c);
symbol_t pack40(const char *buffer, size_t n);
void unpack40(symbol_t symbol, char *buffer, size_t n);

#endif //MATSLISP_SRC_STRINGS_H_
