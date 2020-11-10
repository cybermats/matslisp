//
// Created by mats on 2020-11-08.
//

#ifndef MATSLISP_SRC_FUNCTIONS_H_
#define MATSLISP_SRC_FUNCTIONS_H_

#include <stdlib.h>
#include "object.h"


struct object *fn_quote(struct workspace_t *workspace, struct object *args, struct object *env);
struct object *fn_car(struct workspace_t *workspace, struct object *args, struct object *env);
struct object *fn_cdr(struct workspace_t *workspace, struct object *args, struct object *env);
struct object *fn_cons(struct workspace_t *workspace, struct object *args, struct object *env);
struct object *fn_equal(struct workspace_t *workspace, struct object *args, struct object *env);
struct object *fn_atom(struct workspace_t *workspace, struct object *args, struct object *env);
struct object *fn_cond(struct workspace_t *workspace, struct object *args, struct object *env);
struct object *fn_lambda(struct workspace_t *workspace, struct object *args, struct object *env);
struct object *fn_label(struct workspace_t *workspace, struct object *args, struct object *env);
struct object *fn_env(struct workspace_t *workspace, struct object *args, struct object *env);
struct object *fn_gc(struct workspace_t *workspace, struct object *args, struct object *env);

typedef struct object *(*fn_ptr_type)(struct workspace_t *workspace, struct object*, struct object*);

struct tbl_entry_t {
  char* name;
  fn_ptr_type fptr;
};

enum symbol_fn_t {
  FN_QUOTE = 0,
  FN_CAR = 1,
  FN_CDR = 2,
  FN_CONS = 3,
  FN_EQUAL = 4,
  FN_ATOM = 5,
  FN_COND = 6,
  FN_LAMBDA = 7,
  FN_LABEL = 8,
  FN_ENV = 9,
  FN_GC = 10,
};


const static struct tbl_entry_t lookup_table[] = {
    {"quote", fn_quote},
    {"car", fn_car},
    {"cdr", fn_cdr},
    {"cons", fn_cons},
    {"equal", fn_equal},
    {"atom", fn_atom},
    {"cond", fn_cond},
    {"lambda", fn_lambda},
    {"label", fn_label},
    {"env", fn_env},
    {"gc", fn_gc},
};

const static size_t tbl_entry_size = sizeof(lookup_table) / sizeof (*lookup_table);

size_t built_in(const char* n);
void lookup_built_in(symbol_t symbol, char *buffer, size_t n);



#endif //MATSLISP_SRC_FUNCTIONS_H_
