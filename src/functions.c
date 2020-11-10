//
// Created by mats on 2020-11-08.
//
#include <strings.h>
#include <string.h>
#include "functions.h"
#include "error_handling.h"
#include "memory.h"
#include "io.h"

size_t built_in(const char* n) {
  for (size_t entry = 0; entry < tbl_entry_size; ++entry) {
    if (strcasecmp(n, lookup_table[entry].name) == 0)
      return entry;
  }
  return tbl_entry_size;
}

void lookup_built_in(symbol_t symbol, char *buffer, size_t n) {
  strncpy(buffer, lookup_table[symbol].name, n);
}


struct object *fn_quote(struct workspace_t *workspace, struct object *args, struct object *env) {
  return car(args);
}
struct object *fn_car(struct workspace_t *workspace, struct object *args, struct object *env) {
  return caar(args);
}
struct object *fn_cdr(struct workspace_t *workspace, struct object *args, struct object *env) {
  return cdar(args);
}
struct object *fn_cons(struct workspace_t *workspace, struct object *args, struct object *env) {
  push_root(workspace, args);
  struct object *list = new_cons(workspace, car(args), nil);
  args = cadr(args);
  push_root(workspace, list);
  while (consp(args)) {
    append(workspace, list, car(args));
    args = cdr(args);
  }
  if (args != nil) {
    struct object *ptr;
    for (ptr = list; cdr(ptr) != nil; ptr = cdr(ptr));
    setTail(ptr, args);
  }
  pop_root(workspace);
  pop_root(workspace);
  return list;
}
struct object *fn_equal(struct workspace_t *workspace, struct object *args, struct object *env) {
  struct object *first = car(args);
  struct object *second = cadr(args);
  if (get_name(first) == get_name(second)) {
    return TEE;
  } else {
    return nil;
  }
}
struct object *fn_atom(struct workspace_t *workspace, struct object *args, struct object *env) {
  if (!consp(getHead(args)))
    return TEE;
  else
    return nil;
}
struct object *fn_cond(struct workspace_t *workspace, struct object *args, struct object *env) {
  while(consp(args)) {
    struct object *list = car(args);
    struct object *pred = nil;

    if (car(list) != nil)
      pred = eval(workspace, car(list), env);
    struct object *ret = cadr(list);
    if (pred != nil)
      return eval(workspace, ret, env);
    args = cdr(args);
  }
  return nil;
}

struct object *interleave(struct workspace_t *workspace, struct object *formals, struct object *arguments) {
  if (formals == nil && arguments == nil) {
    return nil;
  }
  struct object *env = nil;
  while (consp(formals)){
    if (!consp(arguments)) {
      error("Too few arguments applied.");
    }
    push_root(workspace, env);
    struct object *tail = new_cons(workspace, car(arguments), nil);
    push_root(workspace, tail);
    struct object *tmp = new_cons(workspace, car(formals), tail);
    push_root(workspace, tmp);
    env = new_cons(workspace, tmp, env);
    pop_root(workspace);
    pop_root(workspace);
    pop_root(workspace);
    formals = cdr(formals);
    arguments = cdr(arguments);
  }
  if (arguments != nil) {
    error("Too many arguments applied.");
  }
  return env;
}

struct object *replace_atom(struct workspace_t *workspace, struct object *sexp, struct object *with) {
  if (consp(sexp)) {
    struct object *head = replace_atom(workspace, car(sexp), with);
    push_root(workspace, head);
    struct object *list = new_cons(workspace, head, nil);
    pop_root(workspace);
    sexp = cdr(sexp);
    push_root(workspace, list);

    while (consp(sexp)) {
      append(workspace, list, replace_atom(workspace, car(sexp), with));
      sexp = cdr(sexp);
    }
    pop_root(workspace);
    return list;
  }
  else {
    struct object *tmp = with;
    while (consp(tmp)) {
      struct object *item = car(tmp);
      struct object *atom = car(item);
      struct object *replacement = cadr(item);

      if (get_name(atom) == get_name(sexp))
        return replacement;
      tmp = cdr(tmp);
    }
    return sexp;
  }
}

struct object *fn_lambda(struct workspace_t *workspace, struct object *args, struct object *env) {
  push_root(workspace, args);
  struct object *lambda = getArg(car(args));
  struct object *arguments = cdr(args);

  struct object *list = interleave(workspace, car(lambda), arguments);
  push_root(workspace, list);
  struct object *sexp = replace_atom(workspace, cdr(lambda), list);
  pop_root(workspace);
  pop_root(workspace);
  push_root(workspace, sexp);
  struct object *tmp = eval(workspace, sexp, env);
  pop_root(workspace);
  return tmp;
}

struct object *fn_label(struct workspace_t *workspace, struct object *args, struct object *env) {
  struct object *head = new_symbol(workspace, get_name(car(args)));
  push_root(workspace, head);
  struct object *tail = new_cons(workspace, cadr(args), nil);
  push_root(workspace, tail);
  struct object *obj = new_cons(workspace, head, tail);
  pop_root(workspace);
  pop_root(workspace);
  append(workspace, env, obj);
  return TEE;
}

struct object *fn_env(struct workspace_t *workspace, struct object *args, struct object *env) {
  return env;
}

struct object *fn_gc(struct workspace_t *workspace, struct object *args, struct object *env) {
  gc(workspace);
  return TEE;
}
