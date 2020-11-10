//
// Created by mats on 2020-11-08.
//

#include <assert.h>
#include "object.h"
#include "memory.h"
#include "functions.h"
#include "strings.h"
#include "error_handling.h"

struct object *TEE = NULL;
struct object *nil = NULL;


struct object* new_symbol(struct workspace_t* workspace, symbol_t name) {
  struct object* obj = obj_alloc(workspace);
  obj->type = SYMBOL;
  obj->name = name;
  return obj;
}
struct object* new_number(struct workspace_t* workspace, int integer) {
  struct object* obj = obj_alloc(workspace);
  obj->type = NUMBER;
  obj->integer = integer;
  return obj;
}

struct object* new_cons(struct workspace_t* workspace, struct object* head, struct object* tail) {
  assert((!head || head->alive) && (!tail || tail->alive));
  struct object* obj = obj_alloc(workspace);
#ifndef NDEBUG
  obj->type = PAIR;
#endif
  setHead(obj, head);
  setTail(obj, tail);
  return obj;
}

struct object *new_lambda(struct workspace_t *workspace, struct object *args, struct object *sexp) {
  assert((args && args->alive) && (sexp && sexp->alive));
  struct object *obj = obj_alloc(workspace);
  obj->type = LAMBDA;
  push_root(workspace, obj);
  setArg(obj, new_cons(workspace, args, sexp));
  pop_root(workspace);
  return obj;
}

int symbolp(struct object *obj) {
  assert(obj && obj->alive);
  if (obj != nil)
    return obj->type == SYMBOL;
  return 0;
}

int numberp(struct object *obj) {
  assert(obj && obj->alive);
  if (obj != nil)
    return obj->type == NUMBER;
  return 0;
}
int consp(struct object *obj) {
  assert(obj && obj->alive);
  if (obj != nil)
    return obj->type > PAIR;
  return 0;
}

int lambdap(struct object *obj) {
  assert(obj && obj->alive);
  if (obj != nil)
    return obj->type == LAMBDA;
  return 0;
}

void render_name(struct object *obj, char *buffer, size_t n) {
  assert(obj && obj->alive);
  symbol_t symbol = get_name(obj);
  if (symbol < tbl_entry_size) {
    lookup_built_in(symbol, buffer, n);
    return;
  }
  unpack40(symbol, buffer, n);
}


struct object *value(struct object *sexp, struct object *env) {
  assert(sexp && env && sexp->alive && env->alive);
  struct object *tmp = env;
  while (consp(tmp)) {
    struct object *item = getHead(tmp);
    if (consp(item) && get_name(getHead(item)) == get_name(sexp))
      return getTail(item);
    tmp = getTail(tmp);
  }
  return nil;
}

struct object *apply(struct workspace_t *workspace, struct object *sexp, struct object *env) {
  if (lambdap(getHead(sexp))) {
    return fn_lambda(workspace, sexp, env);
  } else if (symbolp(getHead(sexp))) {
    symbol_t name = get_name(getHead(sexp));
    if (name < tbl_entry_size) {
      return lookup_table[name].fptr(workspace, getTail(sexp), env);
    }
  }
  return sexp;
}

struct object *evaluate_list(struct workspace_t *workspace, struct object *obj, struct object *env) {
  if (!consp(obj))
    return obj;
  struct object *head = eval(workspace, getHead(obj), env);
  push_root(workspace, head);
  struct object *tail = evaluate_list(workspace, getTail(obj), env);
  push_root(workspace, tail);
  struct object * tmp = new_cons(workspace, head, tail);
  pop_root(workspace);
  pop_root(workspace);
  return tmp;
}

struct object *eval(struct workspace_t *workspace, struct object *sexp, struct object *env) {
  if (sexp == nil)
    return nil;

  if (symbolp(sexp)) {
    struct object *pair = value(sexp, env);
    if (pair == nil)
      return sexp;
    return getHead(pair);
  }
  else if (consp(sexp)) {
    if (symbolp(getHead(sexp)) && get_name(getHead(sexp)) == FN_LAMBDA) {
      struct object *largs = cadr(sexp);
      struct object *lsexp = car(cddr(sexp));
      return new_lambda(workspace, largs, lsexp);
    }
    struct object *tmp = evaluate_list(workspace, sexp, env);
    push_root(workspace, tmp);
    tmp = apply(workspace, tmp, env);
    pop_root(workspace);
    return tmp;
  }
  return sexp;
}

symbol_t get_name(struct object *obj) {
  if (!symbolp(obj))
    error("cannot get name from non symbol.");
  return obj->name;
}

void append(struct workspace_t *workspace, struct object *list, struct object *obj) {
  if (!getHead(list) && !getTail(list)) {
    setHead(list, obj);
    setTail(list, nil);
    return;
  }
  struct object *ptr;
  for (ptr = list; getTail(ptr) != nil; ptr = getTail(ptr));
  push_root(workspace, obj);
  setTail(ptr, new_cons(workspace, obj, nil));
  pop_root(workspace);
}
