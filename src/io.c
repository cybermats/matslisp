//
// Created by mats on 2020-11-08.
//
#include <stdlib.h>
#include "io.h"
#include "object.h"
#include "error_handling.h"
#include "functions.h"
#include "strings.h"
#include "memory.h"

oop read_digits(struct workspace_t* workspace, FILE *fp, int c) {
  static char buffer[1024];
  char* buf_ptr = buffer;
  do {
    *buf_ptr++ = c;
    c = getc(fp);
  } while(is_digit10(c));
  // TODO: Handle hex
  ungetc(c, fp);
  *buf_ptr = 0;
  return new_number(workspace, strtol(buffer, 0, 0));
}

struct object *read_letter(struct workspace_t *workspace, FILE *fp, int c) {
#define buffer_size 1024
  static char buffer[buffer_size];
  char *buf_ptr = buffer;
  buffer[1] = buffer[2] = 0;
  while (is_letter(c) || is_digit10(c)) {
    *buf_ptr++ = c;
    c = getc(fp);
  }
  ungetc(c, fp);
  *buf_ptr = 0;
  symbol_t symbol = built_in(buffer);
  if (symbol < tbl_entry_size)
    return new_symbol(workspace, symbol);
  return new_symbol(workspace, pack40(buffer, buffer_size));
}
oop read(struct workspace_t* workspace, FILE *fp) {
  for (;;) {
    int c = getc(fp);
    switch (c) {
      case EOF: {
        return (oop)EOF;
      }
      case ' ':
      case '\n':
      case '\t': {
        continue;
      }
      case ';': {
        for (;;) {
          c = getc(fp);
          if ('\n' == c || '\r' == c || EOF == c) break;
        }
        continue;
      }
      case '\'': {
        struct object *obj = read(workspace, fp);
        push_root(workspace, obj);
        obj = new_cons(workspace, obj, nil);
        push_root(workspace, obj);
        oop sym = new_symbol(workspace, FN_QUOTE);
        push_root(workspace, sym);
        obj = new_cons(workspace, sym, obj);
        pop_root(workspace);
        pop_root(workspace);
        pop_root(workspace);
        return obj;
      }
      case '0' ... '9':
        return read_digits(workspace, fp, c);
      case '(':
        return read_list(workspace, fp, ')');
      case ')': {
        ungetc(c, fp);
        return (oop)EOF;
      }
      case '-': {
        int d=getc(fp);
        ungetc(d, fp);
        if (is_digit10(d))
          return read_digits(workspace, fp, c);
        // fall through
      }
      default: {
        if (is_letter(c)) {
          return read_letter(workspace, fp, c);
        }
        error("illegal character: 0x%02x '%c'", c, c);
      }
    }
  }
}



struct object *checkEOF(FILE *fp, int delimiter, struct object *head);
oop read_list(struct workspace_t* workspace, FILE *fp, int delimiter) {
  struct object *head = nil, *tail = nil;
  struct object *obj = read(workspace, fp);
  if (obj == (oop)EOF)
    return checkEOF(fp, delimiter, head);
  push_root(workspace, obj);
  head = tail = new_cons(workspace, obj, nil);
  pop_root(workspace);
  push_root(workspace, head);
  for (;;) {
    obj = read(workspace, fp);
    if (obj == (oop)EOF) {
      pop_root(workspace);
      return checkEOF(fp, delimiter, head);
    }
    push_root(workspace, obj);
    obj = new_cons(workspace, obj, nil);
    pop_root(workspace);
    tail = setTail(tail, obj);
  }
}
struct object *checkEOF(FILE *fp, int delimiter, struct object *head) {
  int c = getc(fp);
  if (c != delimiter)
    fatal("EOF while reading list");
  return head;
}


void write(FILE *fp, struct object *obj) {
  if (obj == nil || !obj || (!obj->head && !obj->tail)) {
    fprintf(fp, "()");
    return;
  }
  if (consp(obj)) {
    if (symbolp(car(obj)) && get_name(car(obj)) == FN_QUOTE) {
      fprintf(fp, "\'(");
    } else {
      fprintf(fp, "(");
    }
    for(;consp(obj); obj = cdr(obj)) {
      if (symbolp(car(obj)) && get_name(car(obj)) == FN_QUOTE)
        continue;

      write(fp, car(obj));
      if (cdr(obj) != nil)
        fprintf(fp, " ");
    }
    if (nil != obj) {
      fprintf(fp, ". ");
      write(fp, obj);
    }
    fprintf(fp, ")");
  }
  else if(symbolp(obj)) {
    static char buffer[128];
    render_name(obj, buffer, 128);
    fprintf(fp, "%s", buffer);
  } else if(numberp(obj)) {
    fprintf(fp, "%d", obj->integer);
  } else if (lambdap(obj)) {
    fprintf(fp, "(lambda ");
    write(fp, car(getArg(obj)));
    fprintf(fp, " ");
    write(fp, cdr(getArg(obj)));
    fprintf(fp, ")");
  }

  else {
    fatal("invalid type when printing.");
  }
}
