//
// Created by mats on 2020-11-08.
//

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

jmp_buf exception;

int error_init() {
  return setjmp(exception);
}

void fatal(char *reason, ...) {
  if (reason) {
    va_list ap;
    va_start(ap, reason);
    fprintf(stderr, "\nerror: ");
    vfprintf(stderr, reason, ap);
    fprintf(stderr, "\n");
    va_end(ap);
  }
  exit(1);
}

void error(char *reason, ...) {
  if (reason) {
    va_list ap;
    va_start(ap, reason);
    fprintf(stderr, "\nerror: ");
    vfprintf(stderr, reason, ap);
    fprintf(stderr, "\n");
    va_end(ap);
  }
  longjmp(exception, 1);
}
