//
// Created by mats on 2020-11-08.
//

#ifndef MATSLISP_SRC_ERROR_HANDLING_H_
#define MATSLISP_SRC_ERROR_HANDLING_H_

#include <setjmp.h>
extern jmp_buf exception;
void fatal(const char *reason, ...);
void error(const char *reason, ...);


#endif //MATSLISP_SRC_ERROR_HANDLING_H_
